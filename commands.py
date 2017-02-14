import serial

def run(target_db):
    ser1 = serial.Serial(port='COM10', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)
    ser1.write('X1J'+ str(int(target_db.value_x)) +','+ str(int(target_db.value_y)) + ',' + str(int(target_db.value_z)) + '\r')
    ser1.write('X2J'+ str(int(target_db.value_x)) +','+ str(int(target_db.value_y)) + ',' + str(int(target_db.value_z)) + '\r')
    result = ser1.read(30)
    ser1.close()

    return result
