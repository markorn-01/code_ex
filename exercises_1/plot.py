import matplotlib.pyplot as plt

# Your extracted data
n_values = [10000, 100000, 1000000, 10000000, 100000000, 1000000000]
times =    [0.000258, 0.002279, 0.035000, 0.158616, 1.431443, 14.287778]

plt.figure(figsize=(8,6))
plt.plot(n_values, times, marker='o')
plt.xscale('log')
plt.yscale('log')
plt.title('Running Time vs Number of Points')
plt.xlabel('Number of Points (n)')
plt.ylabel('Running Time (seconds)')
plt.grid(True, which='both', linestyle='--')
plt.tight_layout()
plt.show()

# Running with n = 10000
# Time taken: 0.000258 seconds
# Estimated pi ≈ 3.147600
# Running with n = 100000
# Time taken: 0.002279 seconds
# Estimated pi ≈ 3.140720
# Running with n = 1000000
# Time taken: 0.035000 seconds
# Estimated pi ≈ 3.142992
# Running with n = 10000000
# Time taken: 0.158616 seconds
# Estimated pi ≈ 3.142115
# Running with n = 100000000
# Time taken: 1.431443 seconds
# Estimated pi ≈ 3.141505
# Running with n = 1000000000
# Time taken: 14.287778 seconds
# Estimated pi ≈ 3.141566