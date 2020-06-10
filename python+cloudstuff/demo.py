import random
import urllib.request
import time
# input = 'https://api.thingspeak.com/channels/1079082/fields/1.json?api_key=ENUNMH9IN4SYQW2G&results=2'
import requests


def typeOfVeg(veg):
    switcher = {
        0: '&field1={}&field2={}',
        1: '&field3={}&field4={}',
        2: '&field5={}&field6={}',
    }
    return switcher.get(veg, 'try again')


totalOverall: int = 0
totalTomato: int = 0
totalOnion: int = 0
totalPotato: int = 0

prices = [0,0,0]
# using this try to correct the matlab graph.

"""
def read_data_thingspeak():
    URL='https://api.thingspeak.com/channels/1079082/fields/1.json?'
    # KEY='ENUNMH9IN4SYQW2G'
    HEADER='results=100'
    # NEW_URL=URL+KEY+HEADER
    NEW_URL=URL+HEADER
    print(NEW_URL)

    get_data=requests.get(NEW_URL).json()
    # print("get_data = ", get_data)
    channel_id=get_data['channel']['id']
    data = get_data['feeds']
    # print("data = ", data)

    # t = []
    global totalOverall
    for x in data:
        #print(x['field1'])
        # t.append(x['field1'])
        # print("x = ", x['field1'])
        totalOverall += (int)(x['field1'])

    print("total = ",totalOverall)
"""


def testing():
    # threading.Timer(30, testing).start()
    total = 0
    URL = 'https://api.thingspeak.com/update?api_key='
    KEY = 'K7M9UXMSLQPD5CDO'
    HEADER = ''

    n = int(input("Enter the number of entries:: "))
    i = 0
    total = 0
    while i < n:
        i += 1
        print("Entry ", i, "::\n")
        veg = int(input("Enter the code of the veg:: "))
        wg = int(input("Enter the weight:: "))
        price = wg * prices[veg]
        total += price
        print("price = ", price)

        if veg == 0:
            global totalTomato
            totalTomato += price
            HEADER += typeOfVeg(veg).format(price, totalTomato)
        elif veg == 1:
            global totalOnion
            totalOnion += price
            HEADER += typeOfVeg(veg).format(price, totalOnion)
        elif veg == 2:
            global totalPotato
            totalPotato += price
            HEADER += typeOfVeg(veg).format(price, totalPotato)
        else:
            print("Enter a valid veg code\n\n")
            i -= 1

    totalOverall = totalTomato + totalOnion + totalPotato
    HEADER += '&field7={}&field8={}'.format(total, totalOverall)
    # print(HEADER)
    new_URL = URL + KEY + HEADER
    data = urllib.request.urlopen(new_URL)
    print(new_URL)
    print(data)
    print("TotalOverall = ", totalOverall)
    print("Please wait for 30 seconds")
    time.sleep(30)
    # amt = random.randint(1, 20)
    # HEADER = '&field1={}&field2={}&field3={}&field4={}&field5={}'.format(val, val, val, val, val)


if __name__ == '__main__':
    # read_data_thingspeak()
    # print("Enter the price of T, O, P respectively::")
    prices[0] = int(input("Price of Tomato per kg:: "))
    prices[1] = int(input("Price of Onion per kg:: "))
    prices[2] = int(input("Price of Potato per kg:: "))
    while True:
        testing()
