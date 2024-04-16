import pyvisa, logging, code
from time import sleep

COMMAND_BEEP = "SYSTem:BEEPer"
COMMAND_V_DC = ":MEASure:VOLTage:DC?"
COMMAND_I_AC = ":MEASure:CURRent:AC?"

class RigolDM3000():
    def __init__(self, id:str=None):
        if id is None:
            raise Exception("No ID supplied")
        self.id = id
        self.connection = None

    def __str__(self):
        msg = f"\n========== {__class__.__name__} =========="
        msg += f"\n\tID: {self.id}"
        msg += f"\n\tConnected: {True if self.connection else False}"
        return msg

    def connect(self):
        try:
            print(f"Attempting connection to {__class__.__name__} with ID {self.id}")
            self.rm = pyvisa.ResourceManager()
            self.connection = self.rm.open_resource(self.id)
            self.connection.read_termination = '\n'
            self.connection.write_termination = '\n'
            self.write("CMDSET RIGOL")
            print(f"\tSuccessfully connected RigolDM3000")
            return True
        except Exception as e:
            print(f"\t{e}")
            print(f"\t Could not connect")
            return False

    def write(self, command):
        return self.connection.write(command)

    def read(self, command):
        return self.connection.query(command)

    def reset(self):
        self.write("*RST")

    def get_voltage_DC(self):
        return self.read(":MEASure:VOLTage:DC?")
    
    def get_voltage_AC(self):
        return self.read(":MEASure:VOLTage:AC?")

    def get_current_DC(self):
        return self.read(":MEASure:CURRent:DC?")

    def get_current_AC(self):
        return self.read(":MEASure:CURRent:AC?")

    def beep(self):
        self.write("SYSTem:BEEPer:STATe ON")
        self.write(COMMAND_BEEP)

if __name__ == "__main__":
    potential_devices =  [x for x in pyvisa.ResourceManager().list_resources() if "DM" in x]
    print(f"Potential devices: {potential_devices}")
    if len(potential_devices) == 0 or len(potential_devices) > 1:
        exit()

    dmm = RigolDM3000(potential_devices[0])
    if not dmm.connect():
        exit()
    print(f"ID: {dmm.read('*IDN?')}")
    dmm.beep()

    code.InteractiveConsole(locals=globals()).interact()