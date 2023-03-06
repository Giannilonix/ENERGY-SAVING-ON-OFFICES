from iotronic_lightningrod.modules.plugins import Plugin
import requests
import time
from requests.exceptions import HTTPError
import json
import random
from influxdb import InfluxDBClient
from oslo_log import log as logging

LOG = logging.getLogger(__name__)

class Worker(Plugin.Plugin):

    def __init__(self, uuid, name, q_result, params=None):
        super(Worker, self).__init__(uuid, name, q_result, params)

    def zone_query(self, zone_number):
        url = 'http://zona' + str(zone_number) + '.local/status'
        LOG.info(url)
        try:
            response = requests.get(url, timeout=3)
            # response = requests.get(url)
            LOG.info(response.text)
            json_response = json.loads(response.text)
            # LOG.info(str(json_response))
            # If the response was successful, no Exception will be raised
            response.raise_for_status()
        except HTTPError as http_err:
            LOG.info('HTTP error occurred:' + http_err)  # Python 3.6
            # return http_err
        except Exception as err:
            LOG.info('Other error occurred: ' + err)  # Python 3.6
            # return err
        else:
            LOG.info("Zona " + str(zone_number) + ":" + str(json_response))
        return float(json_response["Temp"]), json_response["Presence"], int(json_response["Lux"])

    def set_actuator(self, actuator_number, type='fc', mode='on'):
        url = 'http://ledcontroller.local/' + type + '_' + str(actuator_number) + "_" + mode
        LOG.info(url)
        try:
            response = requests.get(url, timeout=3)
            # response = requests.get(url)
            # json_response = json.loads(response.text)
            # If the response was successful, no Exception will be raised
            response.raise_for_status()
        except HTTPError as http_err:
            LOG.info('HTTP error occurred: ' + http_err)  # Python 3.6
            # return http_err
        except Exception as err:
            LOG.info('Other error occurred: ' + err)  # Python 3.6
            # return err
        else:
            LOG.info("Attuatore " + type + '_' + str(actuator_number) + mode)
        return response

    def get_actuator_status(self):
        url = 'http://ledcontroller.local/status'
        LOG.info(url)
        try:
            response = requests.get(url, timeout=3)
            # response = requests.get(url)
            json_response = json.loads(response.text)
            LOG.info(response.text)
            # If the response was successful, no Exception will be raised
            response.raise_for_status()
        except HTTPError as http_err:
            LOG.info('HTTP error occurred: ' + http_err)  # Python 3.6
            # return http_err
        except Exception as err:
            LOG.info('Other error occurred: ' + err)  # Python 3.6
            # return err
        else:
            return json_response

    def convertPres(self, pres):
        if pres=='True':
            return 1
        else:
            return 0

    def run(self):

        LOG.info("Plugin " + self.name + " process completed!")
        self.q_result.put("ECHO RESULT: OK")

        # PARAMS FOR DB CONNECTION
        user = "me"
        password = "arancino"
        dbname = "test_da_script"
        host = "localhost"
        port = "8086"

        client = InfluxDBClient(host, port, user, password, dbname)

        # CREATE DB
        client.create_database(dbname)
        client.switch_database(dbname)

        # PARAMS FOR MAIN
        lux_status = [0, 0, 0, 0]
        p_status = ['False', 'False', 'False', 'False']
        index=0

        while (True):

            for i in range(1, 5):

                status = self.get_actuator_status()
                temp, pres, lux = self.zone_query(i)

                index=index+1

                json_body = [{

                    'measurement': 'zone' + str(i),
                    'tags': {
                        'host': 'host' + str(index),
                    },
                    'fields': {
                        'lux': lux,
                        'temp': temp,
                        'pres': self.convertPres(pres)
                    }

                }]

                client.write_points(json_body)

                lux_status[i - 1] = lux
                p_status[i - 1] = pres

                if pres == 'True':
                    # set_actuator(i)
                    # if int(lux) < 400 and status[LIGHT] == 'off':
                    if lux <= 400:
                        if (i == 1 or i == 2) and status["LIGHT_1"] == 'off':
                            self.set_actuator(1, 'l')
                        if (i == 2 or i == 3) and status["LIGHT_2"] == 'off':
                            self.set_actuator(2, 'l')
                        if (i == 3 or i == 4) and status["LIGHT_3"] == 'off':
                            self.set_actuator(3, 'l')
                    else:
                        if (i == 1 or i == 2) and status["LIGHT_1"] == 'on' and lux_status[0] > 400 and lux_status[
                            1] > 400:
                            self.set_actuator(1, 'l', 'off')
                        if (i == 2 or i == 3) and status["LIGHT_2"] == 'on' and lux_status[1] > 400 and lux_status[
                            2] > 400:
                            self.set_actuator(2, 'l', 'off')
                        if (i == 3 or i == 4) and status["LIGHT_3"] == 'on' and lux_status[2] > 400 and lux_status[
                            3] > 400:
                            self.set_actuator(3, 'l', 'off')
                    # if temp in range (15,27) and status["FC_"+str(i)] == 'off':
                    if temp < 18 or temp > 25:
                        LOG.info("Attivo impianto riscaldamento/raffreddamento")
                        self.set_actuator(i)
                    elif status["FC_" + str(i)] == 'on':
                        self.set_actuator(i, 'fc', 'off')

                else:
                    if status["FC_" + str(i)] == 'on':
                        self.set_actuator(i, 'fc', 'off')
                    if (i == 1 or i == 2) and status["LIGHT_1"] == 'on' and p_status[0] == 'False' and p_status[
                        1] == 'False':
                        self.set_actuator(1, 'l', 'off')
                    if (i == 2 or i == 3) and status["LIGHT_2"] == 'on' and p_status[1] == 'False' and p_status[
                        2] == 'False':
                        self.set_actuator(2, 'l', 'off')
                    if (i == 3 or i == 4) and status["LIGHT_3"] == 'on' and p_status[2] == 'False' and p_status[
                        3] == 'False':
                        self.set_actuator(3, 'l', 'off')

                time.sleep(1 / 2)
