import java.util.ArrayList;
import java.util.List;

class QuickStart {
    static List<Integer> getCoins(int amount) {
        boolean prime[] = new boolean[amount+1];
        for(var i=0;i<amount;i++)
            prime[i] = true;

        for(var p = 2; p*p <=amount; p++) {
            if(prime[p] == true) {
                for(var i = p*2; i <= amount; i += p)
                    prime[i] = false;
            }
        }

        var primes = new ArrayList<Integer>();

        // add 1 as a requirement
        primes.add(1);

        for(var i = 2; i <= amount-1; i++) {
            if(prime[i] == true)
                primes.add(i);
        }

        // add amount as golden coin
        primes.add(amount);

        return primes;
    }

    static int findCombos(List<Integer> coins, int startCoinIndex, int amount, int targetAmount) {
        var combos = 0;

        for(var i = startCoinIndex; i < coins.size(); i++) {
            var coin = coins.get(i);
            var newAmount = amount + coin;

            if(newAmount == targetAmount) {
                combos++;
            } else if(newAmount < targetAmount) {
                combos += findCombos(coins, i, newAmount, targetAmount);
            }
        }
        
        return combos;
    }

    static int calculateCoinsAllCombos(int amount) {
        var coins = getCoins(amount);
        return findCombos(coins, 0, 0, amount);
    }

    static int findCombos(List<Integer> coins, int startCoinIndex, int coinsLength, int coinsTargetLength, int amount, int targetAmount) {
        var combos = 0;

        for(var i = startCoinIndex; i < coins.size(); i++) {
            var coin = coins.get(i);
            var newAmount = amount + coin;
            var newCoinsLength = coinsLength + 1;

            if(newCoinsLength == coinsTargetLength && newAmount == targetAmount) {
                combos++;
            } else if(newCoinsLength < coinsTargetLength && newAmount < targetAmount) {
                combos += findCombos(coins, i, newCoinsLength, coinsTargetLength, newAmount, targetAmount);
            }
        }
        
        return combos;
    }

    static int calculateCoinsUsingTargetLength(int amount, int targetLength) {
        var coins = getCoins(amount);
        return findCombos(coins, 0, 0, targetLength, 0, amount);
    }

    static int findCombos(List<Integer> coins, int startCoinIndex, int coinsLength, int minTargetLength, int maxTargetLength, int amount, int targetAmount) {
        var combos = 0;

        for(var i = startCoinIndex; i < coins.size(); i++) {
            var coin = coins.get(i);
            var newAmount = amount + coin;
            var newCoinsLength = coinsLength + 1;

            if(newCoinsLength >= minTargetLength && newCoinsLength <= maxTargetLength && newAmount == targetAmount) {
                combos++;
            } else if(newCoinsLength < maxTargetLength && newAmount < targetAmount) {
                combos += findCombos(coins, i, newCoinsLength, minTargetLength, maxTargetLength, newAmount, targetAmount);
            }
        }
        
        return combos;
    }

    static int calculateCoinsUsingTargetLengthRange(int amount, int minTargetLength, int maxTargetLength) {
        var coins = getCoins(amount);
        return findCombos(coins, 0, 0, minTargetLength, maxTargetLength, 0, amount);
    }

    public static void main (String[] args) {
        var startTime = System.currentTimeMillis();
        var result = calculateCoinsAllCombos(5);
        var endTime = System.currentTimeMillis();
        var totalTime = endTime - startTime;
        var totalTimeSeconds = (double) totalTime / 1000.0;
        System.out.println(result);
        System.out.println("Total time taken: " + totalTimeSeconds + " seconds");
        
        startTime = System.currentTimeMillis();
        result = calculateCoinsUsingTargetLength(300, 12);
        endTime = System.currentTimeMillis();
        totalTime = endTime - startTime;
        totalTimeSeconds = (double) totalTime / 1000.0;
        System.out.println(result);
        System.out.println("Total time taken: " + totalTimeSeconds + " seconds");

        startTime = System.currentTimeMillis();
        result = calculateCoinsUsingTargetLengthRange(300, 10, 15);
        endTime = System.currentTimeMillis();
        totalTime = endTime - startTime;
        totalTimeSeconds = (double) totalTime / 1000.0;
        System.out.println(result);
        System.out.println("Total time taken: " + totalTimeSeconds + " seconds");
    }
}