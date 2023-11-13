# Read energy values from input.txt
with open("BeamEnergy_list_OffRes.txt", "r") as file:
    energy_values = [float(line.strip()) for line in file]

# Sort the energy values in descending order
sorted_energies = sorted(energy_values, reverse=True)

# Write sorted energy values to output.txt
with open("EnergyPoints.txt", "w") as file:
    for energy in sorted_energies:
        file.write(f"{energy}\n")

print("Energies have been sorted and saved in EnergyPoints.txt.")
