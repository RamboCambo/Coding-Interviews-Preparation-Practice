#include <iostream>
#include <vector>
#include <ctime>

// Returns a vector of prime numbers up to the given amount
std::vector<int> get_primes(int amount) {
    std::vector<bool> is_prime(amount + 1, true);

    for (int p = 2; p * p <= amount; p++) {
        if (is_prime[p]) {
            for (int i = p * 2; i <= amount; i += p)
                is_prime[i] = false;
        }
    }

    std::vector<int> primes;
    primes.push_back(1);

    for (int i = 2; i <= amount - 1; i++) {
        if (is_prime[i])
            primes.push_back(i);
    }

    primes.push_back(amount);
    return primes;
}

// Recursively finds the number of combinations of coins that add up to the target amount
int find_combinations(const std::vector<int>& coins, int start_coin_index, int amount, int target_amount) {
    int combos = 0;

    for (int i = start_coin_index; i < coins.size(); i++) {
        int coin = coins[i];
        int new_amount = amount + coin;

        if (new_amount == target_amount) {
            combos++;
            break;
        }
        else if (new_amount < target_amount) {
            combos += find_combinations(coins, i, new_amount, target_amount);
        }
    }

    return combos;
}

// Calculates the number of combinations of coins that add up to the given amount
int calculate_all_combinations(int amount) {
    std::vector<int> coins = get_primes(amount);
    return find_combinations(coins, 0, 0, amount);
}

// Recursively finds the number of combinations of coins that add up to the target amount and have the given length
int find_combinations_with_target_length(const std::vector<int>& coins, int start_coin_index, int coins_length, int coins_target_length, int amount, int target_amount) {
    int combos = 0;

    for (int i = start_coin_index; i < coins.size(); i++) {
        int coin = coins[i];
        int new_amount = amount + coin;
        int new_coins_length = coins_length + 1;

        if (new_coins_length == coins_target_length && new_amount == target_amount) {
            combos++;
            break;
        }
        else if (new_coins_length < coins_target_length && new_amount < target_amount) {
            combos += find_combinations_with_target_length(coins, i, new_coins_length, coins_target_length, new_amount, target_amount);
        }
    }

    return combos;
}

// Calculates the number of combinations of coins that add up to the given amount and have the given length
int calculate_combinations_using_target_length(int amount, int target_length) {
    std::vector<int> coins = get_primes(amount);
    return find_combinations_with_target_length(coins, 0, 0, target_length, 0, amount);
}

// Recursively finds the number of combinations of coins that add up to the target amount and have a length within the given range
int find_combinations_with_target_length_range(const std::vector<int>& coins, int start_coin_index, int coins_length, int min_target_length, int max_target_length, int amount, int target_amount) {
    int combos = 0;

    for (int i = start_coin_index; i < coins.size(); i++) {
        int coin = coins[i];
        int new_amount = amount + coin;
        int new_coins_length = coins_length + 1;

        if (new_coins_length >= min_target_length && new_coins_length <= max_target_length && new_amount == target_amount) {
            combos++;
            break;
        }
        else if (new_coins_length < max_target_length && new_amount < target_amount) {
            combos += find_combinations_with_target_length_range(coins, i, new_coins_length, min_target_length, max_target_length, new_amount,
                target_amount);
        }
    }
    return combos;
}

// Calculates the number of combinations of coins that add up to the given amount and have a length within the given range
int calculate_combinations_using_target_length_range(int amount, int min_target_length, int max_target_length) {
    std::vector<int> coins = get_primes(amount);
    return find_combinations_with_target_length_range(coins, 0, 0, min_target_length, max_target_length, 0, amount);
}

int main() {
    clock_t start_time, end_time;
    double total_time_seconds;
    int result;
    start_time = clock();
    result = calculate_all_combinations(5);
    end_time = clock();
    total_time_seconds = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << result << std::endl;
    std::cout << "Total time taken: " << total_time_seconds << " seconds" << std::endl;

    start_time = clock();
    result = calculate_combinations_using_target_length(300, 12);
    end_time = clock();
    total_time_seconds = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << result << std::endl;
    std::cout << "Total time taken: " << total_time_seconds << " seconds" << std::endl;

    start_time = clock();
    result = calculate_combinations_using_target_length_range(300, 10, 15);
    end_time = clock();
    total_time_seconds = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << result << std::endl;
    std::cout << "Total time taken: " << total_time_seconds << " seconds" << std::endl;

    return 0;
}