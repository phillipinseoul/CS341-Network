import sys
import requests, json

url = sys.argv[1]
name = sys.argv[2]
number = int(sys.argv[3])

pg = requests.get(url + "/hello_world").json()
print(pg["message"])

params = {'name': name}

hash = requests.get(url + "/hash", params=params).json()
hash_res = hash["result"]
print(hash_res)

data = {"name": name, "hash": hash_res, "number": number}

collatz = requests.post(url + "/collatz", data=data).json()

print(collatz['result'])

