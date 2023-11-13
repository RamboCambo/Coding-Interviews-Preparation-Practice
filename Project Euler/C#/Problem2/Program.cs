var sum = 2;

var secondLast = 1;
var last = 2;

while(true) {
    var next = secondLast + last;
    if (next > 4000000) {
        break;
    }
    if (next % 2 == 0) {
        sum += next;
    }
    secondLast = last;
    last = next;
}

Console.WriteLine(sum);