import pandas as pd

# Read data from Excel file
excel_file = "EBeam_current_field.xlsx"  # Replace "data.xlsx" with your Excel file name
df = pd.read_excel(excel_file)

# Access data from specific columns
column_name_1 = "RUN"  # runs
column_name_2 = "B_field[G]"  # DHSTB001 magnet field in Gauss (from ELog)
column_name_3 = "E_Beam[B]"  # beam energy derived directly by the B field
column_name_4 = "I[A]"  # DHSTB001 current in ampere (from ELog)
column_name_5 = "E_Beam[I]"  # Beam energy derived with famous formula of BTF
column_name_6 = "E_BeamDB"  # Beam energy read by Database
column_name_7 = "E_BeamPV"  # PV beam energy scan

# Access data from the specified columns and store them in arrays
column1_data = df[column_name_1].tolist()
column2_data = df[column_name_2].tolist()
column3_data = df[column_name_3].tolist()
column4_data = df[column_name_4].tolist()
column5_data = df[column_name_5].tolist()
column6_data = df[column_name_6].tolist()
column7_data = df[column_name_7].tolist()

# Combine data from both columns into a single list for sorting
combined_data = list(zip(column1_data, column2_data, column3_data, column4_data, column5_data, column6_data, column7_data))

# Sort the combined data based on the first column (Column1 values)
sorted_data = sorted(combined_data, key=lambda x: x[0], reverse=True)

# Separate the sorted data into individual arrays for writing to a file
sorted_column1_data, sorted_column2_data, sorted_column3_data, sorted_column4_data, sorted_column5_data, sorted_column6_data, sorted_column7_data = zip(*sorted_data)

# Write sorted data to ordered.txt file
with open("ExcelBeamEnergies.txt", "w") as file:
    for col1, col2, col3, col4, col5, col6, col7 in zip(sorted_column1_data, sorted_column2_data, sorted_column3_data, sorted_column4_data, sorted_column5_data, sorted_column6_data, sorted_column7_data):
        file.write(f"{col1}\t{col2}\{col3}\t{col4}\{col5}\t{col6}\{col7}\n")

print("Data has been sorted and saved in ordered.txt.")
