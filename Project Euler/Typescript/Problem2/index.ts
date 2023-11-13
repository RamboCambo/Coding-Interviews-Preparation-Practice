let sum: number = 3;

let secondLast: number = 1;
let last: number = 2;

while(true) {
    let next = secondLast + last;
    if (next > 4000000) {
        break;
    }
    if (next % 2 === 0) {
        sum += next;
    }
    secondLast = last;
    last = next;
}

console.log(sum);