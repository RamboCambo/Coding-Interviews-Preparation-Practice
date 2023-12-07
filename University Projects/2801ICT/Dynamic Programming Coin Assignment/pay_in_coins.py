import time

def generate_prime_coins(amount):
    """
    Generate a list of coins consisting of 1, prime numbers up to 'amount' and 'amount' itself.
    """
    is_prime = [True for _ in range(amount + 1)]
    
    for potential_prime in range(2, int(amount**0.5) + 1):
        if is_prime[potential_prime]:
            for multiple in range(potential_prime * 2, amount + 1, potential_prime):
                is_prime[multiple] = False

    primes = [number for number in range(2, amount) if is_prime[number]]
    
    return [1] + primes + [amount]

def find_combinations(coins, start_index, current_sum, target_sum):
    """
    Recursively find all combinations of coins that sum up to target_sum.
    """
    total_combinations = 0
    for i in range(start_index, len(coins)):
        coin = coins[i]
        new_sum = current_sum + coin
        if new_sum == target_sum:
            total_combinations += 1
            break
        elif new_sum < target_sum:
            total_combinations += find_combinations(coins, i, new_sum, target_sum)
    return total_combinations

def count_all_combinations(amount):
    """
    Count all combinations of coins that sum up to 'amount'.
    """
    coins = generate_prime_coins(amount)
    return find_combinations(coins, 0, 0, amount)

def find_combinations_with_target_length(coins, start_index, current_length, target_length, current_sum, target_sum):
    """
    Recursively find all combinations of coins of a specific length that sum up to target_sum.
    """
    total_combinations = 0
    for i in range(start_index, len(coins)):
        coin = coins[i]
        new_sum = current_sum + coin
        new_length = current_length + 1
        if new_length == target_length and new_sum == target_sum:
            total_combinations += 1
            break
        elif new_length < target_length and new_sum < target_sum:
            total_combinations += find_combinations_with_target_length(coins, i, new_length, target_length, new_sum, target_sum)
    return total_combinations

def count_combinations_with_target_length(amount, target_length):
    """
    Count all combinations of coins of a specific length that sum up to 'amount'.
    """
    coins = generate_prime_coins(amount)
    return find_combinations_with_target_length(coins, 0, 0, target_length, 0, amount)

def find_combinations_with_length_range(coins, start_index, current_length, min_length, max_length, current_sum, target_sum):
    """
    Recursively find all combinations of coins within a range of lengths that sum up to target_sum.
    """
    total_combinations = 0
    for i in range(start_index, len(coins)):
        coin = coins[i]
        new_sum = current_sum + coin
        new_length = current_length + 1
        if min_length <= new_length <= max_length and new_sum == target_sum:
            total_combinations += 1
            break
        elif new_length < max_length and new_sum < target_sum:
            total_combinations += find_combinations_with_length_range(coins, i, new_length, min_length, max_length, new_sum, target_sum)
    return total_combinations

def count_combinations_with_length_range(amount, min_length, max_length):
    """
    Count all combinations of coins within a range of lengths that sum up to 'amount'.
    """
    coins = generate_prime_coins(amount)
    return find_combinations_with_length_range(coins, 0, 0, min_length, max_length, 0, amount)

# Test the functions with timings
start_time = time.time()
result = count_all_combinations(5)
elapsed_time = time.time() - start_time
print(f"Result: {result}, Time taken for count_all_combinations: {elapsed_time} seconds")

start_time = time.time()
result = count_combinations_with_target_length(300, 12)
elapsed_time = time.time() - start_time
print(f"Result: {result}, Time taken for count_combinations_with_target_length: {elapsed_time} seconds")

start_time = time.time()
result = count_combinations_with_length_range(300, 10, 15)
elapsed_time = time.time() - start_time
print(f"Result: {result}, Time taken for count_combinations_with_length_range: {elapsed_time} seconds")
