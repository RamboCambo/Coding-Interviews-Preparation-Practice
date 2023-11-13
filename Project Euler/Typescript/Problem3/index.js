var x = 600851475143;
while (x % 2 == 0) {
    x = Math.floor(x / 2);
}
var i = 3;
while (i * i < x) {
    while (x % i == 0) {
        x = Math.floor(x / i);
    }
    i += 2;
}
console.log(x);
