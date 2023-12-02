// Your #include files go here <<<<<<<<<<<<<<

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

// Your class specification goes here <<<<<<<<<<<<<<
class Alist
{
	int default_capacity_ = 8;
	int count_ = 0;
	int capacity_ = 0;
	bool sorted_ = false;
	bool sorted_ascending_ = true;
	int* items_;

	void init()
	{
		// set the consistent properties
		capacity_ = default_capacity_;
		// set the items list
		items_ = create_items(default_capacity_);
	}

	int* create_items(int capacity)
	{
		return new int[capacity];
	}

	void set_new_items(int* new_items)
	{
		// clear the items
		delete[] items_;
		// set them
		items_ = new_items;
	}

	int* copy_items(int size) const;

	int binary_search(int min, int max, int num) const;

public:
	// Your class definitions go here <<<<<<<<<<<<<<
	
	// default
	Alist();
	// destructor
	~Alist();
	// copy
	Alist(const Alist& other);
	// move
	Alist(Alist&& other);
	Alist(bool sorted_ascending);
	Alist(int default_capacity, bool sorted_ascending);
	Alist(const vector<int>& list);
	Alist(const char* file_name);

	// copy assignment
	Alist& operator=(const Alist& list);
	// move assignment
	Alist& operator=(Alist&& other);

	void sort();
	void unsort();
	void extend();
	void shrink();
	void save(const char* file_name) const;
	void resize(int size);
	bool remove(int item);
	int search(int item) const;
	void insert(int item);
	
	friend Alist operator+(const Alist& a1, const Alist& a2);
	friend bool operator==(const Alist& a1, const Alist& a2);
	friend Alist operator&&(const Alist& a1, const Alist& a2);
	friend Alist operator^(const Alist& a1, const Alist& a2);
	friend Alist operator-(const Alist& a1, const Alist& a2);
	friend ostream& operator<<(ostream& lhs, const Alist& a1);
	
	Alist& operator-=(const int item)
	{
		remove(item);
		return *this;
	}

	Alist& operator+=(const int item)
	{
		insert(item);
		return *this;
	}

	int& operator[](int index) const;

	void set_sorted_ascending(bool sorted_ascending)
	{
		sorted_ascending_ = sorted_ascending;
	}

	int get_size() const
	{
		return capacity_;
	}

	int get_count() const
	{
		return count_;
	}

	int get_capacity() const
	{
		return capacity_;
	}

	int get_default_capacity() const
	{
		return default_capacity_;
	}

	int get_sorted() const
	{
		return sorted_;
	}

	int get_sorted_ascending() const
	{
		return sorted_ascending_;
	}

	bool contains(const int item) const
	{
		// if the list isn't empty and the search for the item isn't equal to -1
		return count_ != 0 && search(item) != -1;
	}
};

// Class definitions

Alist::Alist()
{
	init();
}

Alist::~Alist()
{
	delete[] items_;
}

Alist::Alist(const Alist& other)
{
	// copy properties
	sorted_ = other.sorted_;
	sorted_ascending_ = other.sorted_ascending_;
	default_capacity_ = other.default_capacity_;
	capacity_ = other.capacity_;
	count_ = other.count_;

	// copy items
	items_ = other.copy_items(capacity_);
}

Alist::Alist(Alist&& other)
{
	// copy other properties
	sorted_ = other.sorted_;
	sorted_ascending_ = other.sorted_ascending_;
	default_capacity_ = other.default_capacity_;
	capacity_ = other.capacity_;
	count_ = other.count_;

	// steal the list pointer and set other to nullptr
	items_ = other.items_;
	other.items_ = nullptr;
}

Alist::Alist(bool sorted_ascending)
{
	// set sorted ascending property
	sorted_ascending_ = sorted_ascending;
	init();
}

Alist::Alist(int default_capacity, bool sorted_ascending)
{
	// set the custom properties
	default_capacity_ = default_capacity;
	sorted_ascending_ = sorted_ascending;
	init();
}

Alist::Alist(const vector<int>& list)
{
	init();
	// range based for loop add each item
	for (auto item : list)
	{
		insert(item);
	}
}

Alist::Alist(const char* file_name)
{
	// open file
	ifstream file;

	file.open(file_name);
	
	if (!file)
	{
		// normal Alist
		init();
	}
	else
	{
		// copy the properties
		file >> sorted_;
		file >> sorted_ascending_;
		file >> default_capacity_;
		file >> capacity_;
		file >> count_;

		// copy the items
		items_ = create_items(capacity_);

		for (auto i = 0; i < count_; i++)
		{
			// copy value into local variable and set item
			int value;
			file >> value;
			items_[i] = value;
		}

		file.close();
	}
}

Alist& Alist::operator=(const Alist& list)
{
	// delete the items
	delete[] items_;

	// copy the properties
	sorted_ = list.sorted_;
	sorted_ascending_ = list.sorted_ascending_;
	default_capacity_ = list.default_capacity_;
	capacity_ = list.capacity_;
	count_ = list.count_;

	// use the copy items function to create another pointer with the other items memory contents
	items_ = list.copy_items(capacity_);

	// return instance of this
	return *this;
}

Alist& Alist::operator=(Alist&& other)
{
	// delete the old items
	delete[] items_;

	// copy the properties
	sorted_ = other.sorted_;
	sorted_ascending_ = other.sorted_ascending_;
	default_capacity_ = other.default_capacity_;
	capacity_ = other.capacity_;
	count_ = other.count_;

	// steal the list pointer and set the other to nullptr
	items_ = other.items_;
	other.items_ = nullptr;

	// return instance of this
	return *this;
}

int* Alist::copy_items(int size) const
{
	// create new list to hold items
	const auto new_items = new int[size];

	// copy each item across
	for(auto i = 0; i < count_; i++)
	{
		new_items[i] = items_[i];
	}

	// return the pointer
	return new_items;
}

int Alist::binary_search(const int min, const int max, const int num) const
{
	// if max < min, it means the item could not be found, return -1
	if(max < min)
	{
		return -1;
	}

	// find the mid point
	auto mid = (min + max) / 2;

	// if the item at the midpoint is equal to the key, then return the index
	if (items_[mid] == num)
		return mid;

	if (sorted_ascending_)
	{
		// if the mid item is greater than the num, then the number must be to the left of the mid
		if (items_[mid] > num)
			return binary_search(min, mid - 1, num);

		// otherwise if must be to the right
		return binary_search(mid + 1, max, num);
	}
	// otherwise it must be sorted descending
	// if the num is greater than the midpoint, then the number must be to the left (larger numbers to the left)
	if (items_[mid] < num)
		return binary_search(min, mid - 1, num);

	// otherwise it must be on the right (smaller numbers to the right)
	return binary_search(mid + 1, max, num);
}

void Alist::sort()
{
	auto new_items = copy_items(capacity_);

	// use sort for ascending
	if (sorted_ascending_)
		std::sort(new_items, new_items + count_);

	// use sort with greater<int> for descending
	else
		std::sort(new_items, new_items + count_, greater<int>());

	set_new_items(new_items);

	// adjust sorted flag to true
	sorted_ = true;
}

void Alist::unsort()
{
	const auto new_items = copy_items(capacity_);

	// fisher-yates shuffle
	// loop through each item and pick a random item in the list and swap values
	for (auto i = count_ - 1; i >= 1; --i)
	{
		const auto other_index = rand() % (i + 1);

		auto temp = new_items[i];
		new_items[i] = new_items[other_index];
		new_items[other_index] = temp;
	}

	set_new_items(new_items);

	// adjust sorted flag to false
	sorted_ = false;
}

void Alist::extend()
{
	// add the default capacity amount
	capacity_ += default_capacity_;
	const auto new_items = copy_items(capacity_);

	set_new_items(new_items);
}

void Alist::shrink()
{
	// check if the count can be decreased by the default capacity amount and still hold all the items
	if (count_ <= capacity_ - default_capacity_)
	{
		capacity_ -= default_capacity_;
		const auto new_items = copy_items(capacity_);

		set_new_items(new_items);
	}
}

void Alist::save(const char* file_name) const
{
	// open file
	ofstream list_file;

	list_file.open(file_name);

	// write properties to the list first
	list_file << sorted_ << endl;
	list_file << sorted_ascending_ << endl;
	list_file << default_capacity_ << endl;
	list_file << capacity_ << endl;
	list_file << count_ << endl;

	// write all the items on a new line each
	for (auto i = 0; i < count_; i++)
	{
		list_file << items_[i] << endl;
	}

	list_file.close();
}

void Alist::resize(const int size)
{
	// create the new items of size given
	const auto new_items = create_items(size);

	// decide whether to add items up to size given, or if the count is low enough to the count
	auto count_to_index = size < count_ ? size : count_;

	// add all the items of all that can fit
	for (auto i = 0; i < count_to_index; ++i)
	{
		new_items[i] = items_[i];
	}

	// set new properties of the list
	capacity_ = size;
	count_ = count_to_index;

	set_new_items(new_items);
}

bool Alist::remove(const int item)
{
	// find the index of the item
	const auto item_index = search(item);

	// if it isn't found, return false
	if (item_index == -1)
		return false;

	if (sorted_)
	{
		const auto new_items = create_items(capacity_);

		// loop through until the item index and add each item to the new list
		for (auto i = 0; i < item_index; i++)
		{
			new_items[i] = items_[i];
		}
		// add each item after the index to the list
		for (auto i = item_index + 1; i < count_; i++)
		{
			new_items[i - 1] = items_[i];
		}

		// decrease the count
		count_--;

		set_new_items(new_items);

		shrink();
	}
	// remove the first occurance of the item and replace the element at the end of the list
	// decrease the count by one so it effectively moves the last element out of scope and so it can be overridden
	// try shrink list
	else
	{
		items_[item_index] = items_[--count_];

		shrink();
	}

	return true;
}

int Alist::search(const int item) const
{
	// binary search if sorted
	if (sorted_)
	{
		return binary_search(0, count_ - 1, item);
	}

	// loop through each item and if the item matches, return the index
	for (auto i = 0; i < count_; ++i)
	{
		if (items_[i] == item)
			return i;
	}
	
	// return -1 otherwise indicating no index was found
	return -1;
}

void Alist::insert(const int item)
{
	// if the number of items is at capacity, extend it before inserting something
	if (count_ == capacity_)
		extend();

	// if it isn't sorted, just insert item to the end of the list and increase the count
	if (!sorted_)
		items_[count_++] = item;
	// if it is sorted
	else
	{
		auto inserted = false;

		const auto new_items = create_items(capacity_);

		// start out count because the list length will increase by one
		auto new_index = count_;

		// loop through the list backwards
		for (auto i = count_ - 1; i >= 0; --i)
		{
			// when the item isn't inserted and it reaches the first item it is
			// greater than or equal to, insert the new item
			// always insert the current item
			if (sorted_ascending_)
			{
				if (!inserted && item >= items_[i])
				{
					new_items[new_index--] = item;
					inserted = true;
				}
				new_items[new_index--] = items_[i];
			}
			// when the item isn't inserted and it reaches the first item it is
			// less than or equal to, insert the new item
			// always insert the current item
			else
			{
				if (!inserted && item <= items_[i])
				{
					new_items[new_index--] = item;
					inserted = true;
				}
				new_items[new_index--] = items_[i];
			}
		}

		// if it has not been inserted, it must be less than everything else in ascending order
		// or if must be greater than everything in descending order
		// insert at the beginning of the list
		if (!inserted)
			new_items[0] = item;

		count_++;

		set_new_items(new_items);
	}
}

int& Alist::operator[](const int index) const
{
	// define int which is for not found
	auto not_found = -1;

	// if the index is less than 0
	if (index < 0)
	{
		cout << "Error! Index " << index << " is less than 0." << endl;
		return not_found;
	}
	// if it is greater than or equal to the count
	if (index >= count_)
	{
		cout << "Error! Index " << index << " is greater than the count." << endl;
		return not_found;
	}

	// return the element
	return items_[index];
}

bool operator==(const Alist& a1, const Alist& a2)
{
	// compare the properties
	auto both_sorted = a1.get_sorted() == a2.get_sorted();
	auto both_ascending = a1.get_sorted_ascending() == a2.get_sorted_ascending();
	auto same_default_capacity = a1.get_default_capacity() == a2.get_default_capacity();
	auto same_size = a1.get_size() == a2.get_size();
	auto same_count = a1.get_count() == a2.get_count();

	// return false if properties aren't equal, so no need to check contents
	if (!(both_sorted && both_ascending && same_default_capacity && same_size && same_count))
		return false;

	// since the counts are equal, we can compare the memory at both to see if they're the same
	for(auto i = 0; i < a1.get_count(); i++)
	{
		// if not equal, return false
		if (a1[i] != a2[i])
			return false;
	}
	
	return true;
}

bool operator!=(const Alist& a1, const Alist& a2)
{
	return !(a1 == a2);
}

Alist operator+(const Alist& a1, const Alist& a2)
{
	Alist result;

	// insert each element into the result from list 1
	for(auto i = 0; i < a1.get_count(); i++)
	{
		result.insert(a1[i]);
	}

	// insert each element into the result from list 2
	for (auto i = 0; i < a2.get_count(); i++)
	{
		result.insert(a2[i]);
	}
	
	return std::move(result);
}

ostream& operator<<(ostream& lhs, const Alist& a1)
{
	// pass each element to the stream
	for(auto i = 0; i < a1.count_; i++)
	{
		// if this is the last element, don't pass a comma to the stream
		if(i == a1.count_ - 1)
		{
			lhs << a1[i];
		}
		// pass other elements to stream with comma
		else
		{
			lhs << a1[i] << ", ";
		}
	}

	return lhs;
}

Alist operator&&(const Alist& a1, const Alist& a2)
{
	Alist result;

	// add every element from list 1 which is not in the result and is in list 2
	for(auto i = 0; i < a1.get_count(); i++)
	{
		if(!result.contains(a1[i]) && a2.contains(a1[i]))
		{
			result.insert(a1[i]);
		}
	}

	return std::move(result);
}

Alist operator^(const Alist& a1, const Alist& a2)
{
	Alist result;

	// add every element from list 1 which isn't in the result list already or in list 2
	for (auto i = 0; i < a1.get_count(); i++)
	{
		if(!result.contains(a1[i]) && !a2.contains(a1[i]))
		{
			result.insert(a1[i]);
		}
	}
	// since every element in the result must not be in list 2, then we can avoid checking
	// if the result contains it, however we must check list 1 does not contain it
	for (auto i = 0; i < a2.get_count(); i++)
	{
		if (!a1.contains(a2[i]))
		{
			result.insert(a2[i]);
		}
	}

	return std::move(result);
}

Alist operator-(const Alist& a1, const Alist& a2)
{
	Alist result;

	// add every element from list 1 to result
	for (auto i = 0; i < a1.get_count(); i++)
	{
		result.insert(a1[i]);
	}

	// try remove every element from result that is in list 2
	for (auto i = 0; i < a2.get_count(); i++)
	{
		result.remove(a2[i]);
	}
	
	return std::move(result);
}

// main function to test Alist class
int main()
{
	// Test basic constructors and += overloading
	Alist a1, a2(true), a3(72, false), a4, a6, a7, a8;
	const int nums_count = 100;
	int n;
	for (int i = 0; i < nums_count; i++)
	{
		n = rand() % 1000;
		a1 += n;
		a2 += n;
		a3 += n;
	}
	cout << "Sizes: " << a1.get_size() << " " << a2.get_size() << " " << a3.get_size() << endl;
	cout << "Counts: " << a1.get_count() << " " << a2.get_count() << " " << a3.get_count() << endl;

	// Test sorting
	bool sorted = true;
	for (int i = 1; i < a2.get_count() && sorted; i++)
		if (a2[i - 1] > a2[i])
			sorted = false;
	if (!sorted)
		cout << "a2 not initialised to sorted" << endl;
	a1.sort();
	for (int i = 1; i < nums_count; i++)
		if (a1[i - 1] > a1[i])
		{
			cout << "a1 not sorted" << endl;
			break;
		}
	a2.unsort();
	sorted = true;
	for (int i = 1; i < a2.get_count() && sorted; i++)
		if (a2[i - 1] > a2[i])
			sorted = false;
	if (sorted)
		cout << "Unsorted failed" << endl;

	// Test explicit methods and -= overloading
	n = a1[20];
	cout << "Double remove: " << a1.remove(n) << " " << a1.remove(n) << endl;
	cout << "Search present, not present: " << a1.search(a1[50]) << " " << a1.search(n) << endl;
	a1 += n;
	cout << "Search (should be 20 57): " << a1.search(n) << " " << a2.search(n) << endl;
	a1.extend();
	cout << "Extend (should be 112): " << a1.get_size() << endl;
	a1.shrink();
	cout << "Shrink (should be 104): " << a1.get_size() << endl;
	n = a1.get_size();
	for (int i = 0; i < n - 6; i++)
		a1 -= a1[0];
	cout << "Remove (should be 8): " << a1.get_size() << endl;
	a3.save("nums.txt");
	Alist a5("nums.txt");
	if (a3 != a5)
		cout << "save / reload failed" << endl;

	// Test copy constructors and resize
	a4 = a3;
	if (a4 != a3)
		cout << "Copy Constructor 1 Fail" << endl;
	a4.remove(a4[50]);
	if (a4 == a3)
		cout << "Copy Constructor 2 Fail" << endl;
	a4.resize(50);
	cout << "Resize (should be 50 50): " << a4.get_size() << " " << a4.get_count() << endl;
	
	// Test other operator overloading
	n = a1[-20];
	n = a1[2 * nums_count];
	for (int i = 0; i < 20; i += 2)
		a6 += i;
	for (int i = 1; i < 20; i += 2)
		a7 += i;
	a5 = a6 + a7;
	a5.sort();
	for (int i = 0; i < 20; i++)
		if (a5[i] != i)
			cout << "+ operator failed" << endl;
	a8 = a6 && a7;
	if (a8.get_count() != 0)
		cout << "&& operator failed 1" << endl;
	a8 = a6 && a6;
	if (a8.get_count() != a6.get_count())
		cout << "&& operator failed 2" << endl;
	a8 = a6 ^ a7;
	if (a8.get_count() != 20)
		cout << "^ operator failed 1" << endl;
	a8 = a6 ^ a6;
	if (a8.get_count() != 0)
		cout << "^ operator failed 2" << endl;
	a8 = a5 - a7;
	a8.sort();
	for (int i = 0; i < 20; i += 2)
		if (a8[i / 2] != i)
			cout << "- operator failed" << endl;
	a7 += 28;
	if (a7.search(28) == -1)
		cout << "+= operator failed" << endl;
	a7 -= 19;
	if (a7.search(19) != -1)
		cout << "-= operator failed" << endl;

	// Test other constructors  
	Alist a9(a8);
	a9.remove(8);
	if (a9.get_count() != a8.get_count() - 1)
		cout << "Constructor 1 Fail" << endl;
	cout << a8 << endl;
	cout << a9 << endl;
	vector<int> vn{0, 1, 2, 3, 4};
	Alist a10(vn);
	for (int i = 0; i < vn.size(); i++)
		if (a10[i] != vn[i])
			cout << "Constructor 2 Fail" << endl;
	return 0;
}
