import requests
import time
from requests.exceptions import HTTPError
import json
import random

def zone_query(zone_number):
    url = 'http://zona'+str(zone_number)+'.local/status'
    print(url)
    try:
        response = requests.get(url, timeout=3)
        #response = requests.get(url)
        json_response = json.loads(response.text)
        #print(str(json_response))
        # If the response was successful, no Exception will be raised
        response.raise_for_status()
    except HTTPError as http_err:
        print("HTTP error occurred: ", http_err)
        #return http_err
    except Exception as err:
        print("Other error occurred:", err)
        #return err
    else:
        print("Zona " + str(zone_number) + ":" + str(json_response))
    return float(json_response["Temp"]),json_response["Presence"],int(json_response["Lux"])

def set_actuator(actuator_number, type = 'fc', mode = 'on'):
    url = 'http://ledcontroller.local/' + type + '_' + str(actuator_number) +"_"+ mode
    print(url)
    try:
        response = requests.get(url, timeout=3)
        #response = requests.get(url)
        #json_response = json.loads(response.text)
        # If the response was successful, no Exception will be raised
        response.raise_for_status()
    except HTTPError as http_err:
        print("HTTP error occurred: ",http_err)
        #return http_err
    except Exception as err:
        print("Other error occurred:",err)
        #return err
    else:
        print("Attuatore " + type + '_' + str(actuator_number) + mode)
    return response

def get_actuator_status():
    url = 'http://ledcontroller.local/status'
    try:
        response = requests.get(url, timeout=3)
        #response = requests.get(url)
        json_response = json.loads(response.text)
        print(response.text)
        # If the response was successful, no Exception will be raised
        response.raise_for_status()
    except HTTPError as http_err:
        print("HTTP error occurred: ", http_err)
        #return http_err
    except Exception as err:
        print("Other error occurred:", err)
        #return err
    else:
        return json_response

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    """ for i in range(1, 4):
        url = "http://zona" + str(i) + ".local/timer_5m"
        response = requests.get(url)
        print(response.text) """
    while(True):
        for i in range(1,5):
            status = get_actuator_status()
            temp,pres,lux = zone_query(i)
            #print("sensors:",temp,pres,lux)
            if pres == 'False':
                #set_actuator(i)
                #if int(lux) < 400 and status[LIGHT] == 'off':
                if lux <= 400:
                    if i in range(1,2) and status["LIGHT_1"] == 'off':
                        set_actuator(1,'l')
                    if i in range(2,3) and status["LIGHT_2"] == 'off':
                        set_actuator(2, 'l')
                    if i in range(4,5) and status["LIGHT_3"] == 'off':
                        set_actuator(3, 'l')
                else:
                    if i in range(1, 2) and status["LIGHT_1"] == 'on':
                        set_actuator(1, 'l','off')
                    if i in range(3, 4) and status["LIGHT_2"] == 'on':
                        set_actuator(2, 'l', 'off')
                    if i in range(4, 5) and status["LIGHT_3"] == 'on':
                       set_actuator(3, 'l', 'off')
                            #if temp in range (15,27) and status["FC_"+str(i)] == 'off':
                #if temp in range(10, 30):
                if temp < 18 or temp > 25:
                    print("Entratoo")
                    set_actuator(i)
                elif status["FC_"+str(i)] == 'on':
                    set_actuator(i, 'fc', 'off')

            else:
                if status["FC_"+str(i)] == 'on':
                    set_actuator(i, 'fc', 'off')
                if i in range(1, 2) and status["LIGHT_1"] == 'on':
                    set_actuator(1, 'l','off')
                if i in range(3, 4) and status["LIGHT_2"] == 'on':
                   set_actuator(1, 'l', 'off')
                if i in range(4, 5) and status["LIGHT_3"] == 'on':
                   set_actuator(1, 'l', 'off')
            time.sleep(2)
"""
url = 'http://zona' + str(1) + '.local/status'
    print(url)
    try:
        response = requests.get(url, timeout=3)
        json_response = json.loads(response.text)
        print(str(json_response['Temp']))

        # If the response was successful, no Exception will be raised
        response.raise_for_status()
    except HTTPError as http_err:
        print('HTTP error occurred: {http_err}')  # Python 3.6
    except Exception as err:
        print('Other error occurred: {err}')  # Python 3.6
    #else:
        #print("Zona " + str(zone_number) + ":" + str(data))
"""


"""
    for i in range(1,4):
        url = "http://zona"+str(i)+".local/timer_5m"
        response = requests.get(url)
        print(response.text)
    """
