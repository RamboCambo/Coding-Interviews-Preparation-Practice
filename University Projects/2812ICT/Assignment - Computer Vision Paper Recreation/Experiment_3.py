import cv2 as cv
import numpy as np
import h5py
# from threading import Thread

# https://docs.opencv.org/3.4/d7/d4d/tutorial_py_thresholding.html

def calculate_adaptive(args):
    gray_images = args['gray_images']
    kernel = args['kernel']
    multiplier = args['multiplier']
    rgb_images = args['rgb_images']
    true_masks = args['true_masks']
    neighbourhood = args['neighbourhood']
    image_masks = []
    
    print("Starting adaptive thresholding with neighbourhood", neighbourhood)
    
    for i in range(gray_images.shape[0]):
        blurred_image = cv.GaussianBlur(gray_images[i],(5,5),0)
        
        ret, img = cv.threshold(blurred_image,127,255,cv.THRESH_BINARY)
        
        otsu_image = cv.adaptiveThreshold(img, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, cv.THRESH_BINARY, neighbourhood, 2)
        otsu_image = cv.morphologyEx(otsu_image, cv.MORPH_OPEN, kernel, iterations=2)
        
        sure_bg = cv.dilate(otsu_image, kernel, iterations=3)
        
        dist = cv.distanceTransform(otsu_image, cv.DIST_L2, 5)
        
        ret, sure_fg = cv.threshold(dist, multiplier * dist.max(), 255, cv.THRESH_BINARY)
        sure_fg = sure_fg.astype(np.uint8)
        
        unknown = cv.subtract(sure_bg, sure_fg)

        ret, markers = cv.connectedComponents(sure_fg)
        
        markers += 1
        markers[unknown == 255] = 0
        
        markers = cv.watershed(rgb_images[i], markers)

        labels = np.unique(markers)

        binary_masks = []
        for label in labels[2:]:
            target = np.where(markers == label, 255, 0).astype(np.uint8)
            binary_masks.append(target)
        
        image_masks.append(binary_masks)
    
    print("Finished adaptive thresholding with neighbourhood", neighbourhood)
    
    weighted_ious = []
    unweighted_ious = []

    for i in range(0, len(true_masks)):
        a = true_masks[i]
        b = image_masks[i]
        
        ious = []
        areas = []
        
        for tm in a:
            area = tm.sum()    
            max_iou = 0
            
            for im in b:
                intersection = np.logical_and(tm, im).sum()
                union = np.logical_or(tm, im).sum()
                iou = intersection / union
                
                if iou > max_iou:
                    max_iou = iou
            
            ious.append(max_iou)
            areas.append(area)
        
        ious = np.array(ious)
        areas = np.array(areas)

        weighted_iou = np.sum(np.dot(ious, areas)) / np.sum(areas)
        unweighted_iou = np.mean(ious)
        
        weighted_ious.append(weighted_iou)
        unweighted_ious.append(unweighted_iou)
        
    print(neighbourhood, 'Adaptive Weighted IOU Average:', round(np.mean(weighted_ious) * 100, 2))
    print(neighbourhood, 'Adaptive Unweighted IOU Average:', round(np.mean(unweighted_ious) * 100, 2))


if __name__ == '__main__':
    f = h5py.File('nyu_depth_v2_labeled.mat', 'r')

    rgb_images = f['images']
    instances = f['instances']
    labels = f['labels']

    rgb_images = np.array(rgb_images)
    instances = np.array(instances)
    labels = np.array(labels)

    instances = np.swapaxes(instances, 1, 2)
    labels = np.swapaxes(labels, 1, 2)

    rgb_images = np.moveaxis(rgb_images, 1, -1)
    rgb_images = np.swapaxes(rgb_images, 1, 2)

    gray_images = np.mean(rgb_images, axis=-1).astype(np.uint8)

    labels = labels.astype(np.float64)
    instances = instances.astype(np.float64)
    instances = instances / 100.0
    
    combined = instances + labels
    
    true_masks = []
    # loop through the number of instances
    for i in range(0, len(combined)):
        masks = []
        
        # get list of unique values in this image
        unique = np.unique(combined[i])
        
        # loop through the unique values
        for j in range(1, len(unique)):
            # create a mask which is True where the instance is equal to j
            mask = np.where(combined[i] == unique[j], 255, 0).astype(np.uint8)
            
            # check if the mask is empty
            if mask.sum() == 0:
                continue
            
            # append the mask to true_masks
            masks.append(mask)
        
        # append masks to true_masks
        true_masks.append(masks)

    image_masks = []

    kernel = cv.getStructuringElement(cv.MORPH_RECT, (3, 3))

    threads = []

    for neighbourhood in [3, 5, 11, 17, 23, 31, 37, 45]:
        args = {
            "gray_images": gray_images,
            "rgb_images": rgb_images,
            "true_masks": true_masks,
            "kernel": kernel,
            "multiplier": 0.1,
            "neighbourhood": neighbourhood,
        }
        
        # create a thread to calculate the IOU
        calculate_adaptive(args)