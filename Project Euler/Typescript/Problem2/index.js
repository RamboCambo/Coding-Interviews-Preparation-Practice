"use strict";
let sum = 3;
let secondLast = 1;
let last = 2;
while (true) {
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
