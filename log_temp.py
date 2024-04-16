from  Rigol_DM3000 import  RigolDM3000 
import sys
import csv
import time
from datetime import datetime, timezone
from thermocouples_reference import thermocouples


def main(): 
    dmm = RigolDM3000('USB0::0x1AB1::0x09C4::DM3R231500949::INSTR')
    if not dmm.connect():
        print("Cannot connect")
        sys.exit(1)


    typeK = thermocouples['K']
    T_cold_junction = 24.0

    # Create a new CSV file with timestamp in the filename
    filename = datetime.now(timezone.utc).strftime("logs/voltage_log_%Y-%m-%d_%H-%M-%S.csv")
    
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file)
        # Write the header of the CSV file
        writer.writerow(['Timestamp (UTC)', 'Voltage (V)', 'Temperature  (C)', 'Cold  Junction Temperature (C)'])


    try:
        while True:
            time.sleep(1)

            timestamp = datetime.now(timezone.utc)
            utc_time = timestamp.replace(tzinfo=timezone.utc) 
            utc_timestamp = utc_time.timestamp() 

            voltage = float(dmm.get_voltage_DC())

            temperature = typeK.inverse_CmV(voltage * 1000, Tref=T_cold_junction)

            with open(filename, 'a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow([utc_timestamp, voltage, temperature, T_cold_junction])
    except ValueError as e:
        print(e)
            
    except KeyboardInterrupt:
        print('Measurement stopped by user')
    
        


if __name__ == "__main__":
    main()
