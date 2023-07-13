import os
import ctypes
import numpy as np
import time

os.system("gcc -shared -o neonlib.so neonlib.c -mfpu=neon -ftree-vectorize")

add_lib = ctypes.CDLL('./neonlib.so')

add_lib.add_neon.argtypes = (ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float))
add_lib.add_neon.restype = None

add_lib.add_cpu.argtypes = (ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float))
add_lib.add_cpu.restype = None

N = 80000

a = np.random.randn(N).astype(np.float32)
b = np.random.randn(N).astype(np.float32)

a_n = a; b_n = b
r_n = np.empty_like(a_n).astype(np.float32)

a_c = a; b_c = b
r_c = np.empty_like(a_c).astype(np.float32)

a_n_ptr = a_n.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
b_n_ptr = a_n.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
r_n_ptr = r_n.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

a_c_ptr = a_c.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
b_c_ptr = a_c.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
r_c_ptr = r_c.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

rr = None

start_time = time.perf_counter_ns()
add_lib.add_neon(ctypes.c_int(len(a)), a_n_ptr, b_n_ptr, r_n_ptr)
end_time = time.perf_counter_ns()
execution_time_neon = end_time - start_time

start_time = time.perf_counter_ns()
add_lib.add_cpu(ctypes.c_int(len(a)), a_c_ptr, b_c_ptr, r_c_ptr)
end_time = time.perf_counter_ns()
execution_time_cpu = end_time - start_time

start_time = time.perf_counter_ns()
rr = a + b
end_time = time.perf_counter_ns()
execution_time_numpy = end_time - start_time

print(f"numpy Time          : {execution_time_numpy/1000} ms")
print(f"neon Time           : {execution_time_neon/1000} ms")
print(f"cpu Time            : {execution_time_cpu/1000} ms")
print(f"error(numpy - neon) : {np.abs(rr-r_n).sum()/len(a)}")
print(f"error(numpy - cpu)  : {np.abs(rr-r_c).sum()/len(a)}")
