import matplotlib.pyplot as plt
import numpy as np
import subprocess
import sys
from pathlib import Path


def to_float_lst(set: list[str]) -> list[float]:
    s = [None] * len(set)
    i = 0;
    for v in set:
        s[i] = float(v)
        i += 1
    return s


def to_str_lst(set: list[float]) -> list[str]:
    s = [None] * len(set)
    i = 0;
    for v in set:
        s[i] = str(v)
        i += 1
    return s


k_factor = sys.argv[1]

np.random.seed(0)


def gen_rnd_sensor_data(low: float,
                        high: float,
                        n_incr: int,
                        n_decr: int) -> np.ndarray:
    incr = np.random.uniform(low=low, high=high, size=n_incr)
    incr.sort()
    decr = np.random.uniform(low=low, high=high, size=n_decr)
    decr[::-1].sort()
    return np.concatenate((incr, decr))


# dataset = sys.argv[2:]
dataset = gen_rnd_sensor_data(0, 1024, 5000, 10000)
dataset = [i / 1024 for i in dataset]
# dataset = [random.randint(1, 1000) for _ in range(100)]
# dataset = np.sin(100).tolist()


# cycles = 20 # how many sine cycles
# resolution = 50000 # how many datapoints to generate

# length = np.pi * 2 * cycles
# dataset = np.sin(np.arange(0, length, length / resolution)).tolist()
# for i in range(0, len(dataset)):
#     dataset[i] = dataset[i] + 10


# dataset=[]
# for i in range(100):
#     dataset.append((i**.25)**2)


cmd = Path.cwd() / Path("a.out")
op = []
op.append(cmd)
op.append(str(k_factor))
op.extend(to_str_lst(dataset))

result_str = subprocess.Popen(op, stdout=subprocess.PIPE).communicate()[0]
filter_response = result_str.split()

print(dataset)
print(filter_response)

assert (len(dataset) == len(filter_response))

x = range(len(dataset))
y1 = to_float_lst(dataset)
y2 = to_float_lst(filter_response)

plt.plot(x, y1, color='b', label="raw")
plt.plot(x, y2, color='r', label="filtered")

plt.scatter(x, y1)
plt.scatter(x, y2)

# plt.autoscale(enable=True, axis='y')
# plt.autoscale(enable=True, axis='x')

# plt.xlim((0, 20))
# plt.ylim((0, 50))

plt.xlabel('x')
plt.ylabel('y')
plt.title('raw -> filter')

# plt.tight_layout()  # otherwise the right y-label is slightly clipped

plt.legend()
plt.show()
