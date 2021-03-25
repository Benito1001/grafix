import numpy as np
import matplotlib.pyplot as plt

t_ray, a_ray = np.transpose(np.loadtxt(f"../Hadata.dat"))

diffa = np.diff(a_ray)
i_list = []
for i, diff in enumerate(diffa):
	if abs(diff) > 100:
		if len(i_list) == 0 or i_list[-1] != i-1:
			i_list.append(i)

min_t = t_ray[int((i_list[0]-1)*0.9)]
max_t = t_ray[int((i_list[1]+1)*1.1)]

fig, axs = plt.subplots(3, figsize=(10, 8), sharex=True)
for i, name in enumerate(["Ha", "Ma", "La"]):
	t_ray, a_ray = np.transpose(np.loadtxt(f"../{name}data.dat"))
	axs[i].plot(t_ray, a_ray, ".", markersize=2)
	axs[i].set_xlim(min_t, max_t)
	axs[i].set_title(f"dt=1e-{4-i}")
	axs[i].set_ylabel("a [$m/s^2$]")
plt.xlabel("t [$s$]")
plt.savefig(f"finfig.png", dpi=200)
