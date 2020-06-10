import random
import urllib.request
import time
# input = 'https://api.thingspeak.com/channels/1079082/fields/1.json?api_key=ENUNMH9IN4SYQW2G&results=2'
import requests


def typeOfVeg(veg):
    switcher = {
        0: '&field1={}',
        1: '&field2={}',
        2: '&field3={}',
        3: '&field4={}',
        4: '&field5={}',
    }
    return switcher.get(veg, 'try again')


total: int = 0
# using this try to correct the matlab graph.


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
    global total
    for x in data:
        #print(x['field1'])
        # t.append(x['field1'])
        # print("x = ", x['field1'])
        total += (int)(x['field1'])

    print("total = ",total)


def testing():
    # threading.Timer(30, testing).start()
    URL = 'https://api.thingspeak.com/update?api_key='
    KEY = 'XVH405UG4E67L345'
    veg = int(input("Enter the code of the veg::"))
    amt = int(input("Enter the amount::"))

    global total
    total += amt
    HEADER = typeOfVeg(veg).format(amt) + '&field7={}'.format(total)
    new_URL = URL + KEY + HEADER
    data = urllib.request.urlopen(new_URL)
    print(new_URL)
    print(data)

    # HEADER = '&field7={}'.format(total)
    # new_URL = URL + KEY + HEADER
    # data = urllib.request.urlopen(new_URL)

    print("Total = ", total)
    print(new_URL)
    print(data)
    print("Please wait for 30 seconds")
    time.sleep(30)
    # amt = random.randint(1, 20)
    # HEADER = '&field1={}&field2={}&field3={}&field4={}&field5={}'.format(val, val, val, val, val)


if __name__ == '__main__':
    read_data_thingspeak()
    while True:
        testing()
