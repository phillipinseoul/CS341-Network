import sys
import requests, json

url = sys.argv[1]
name = sys.argv[2]
number = int(sys.argv[3])

### 1. Request message from "/hello_world" ###
# Receive message by using GET method
msg = requests.get(url + "/hello_world").json()
print(msg["message"])

### 2. Request "/hash" for SHA256 hashed value ###
# Put 'name' variable into a key-value pair
params = {'name': name}

# Receive hash value by using GET method
hash = requests.get(url + "/hash", params=params).json()
hash_res = hash["result"]
print(hash_res)

### 3. Request "/collatz" for next collatz number ###
# Put variables into key-value pairs
data = {"name": name, "hash": hash_res, "number": number}

# Receive collatz number by using POST method
collatz = requests.post(url + "/collatz", data=data).json()

# Error Handling: If E1 or E2, print error message
if "error" in collatz:
    print(collatz['error'])
    res = 1
else:
    res = collatz['result']
    print(res)

# Keep requesting the next collatz number until the result is 1
while res != 1:
    data["number"] = res
    collatz = requests.post(url + "/collatz", data=data).json()
    res = collatz['result']
    print(res)
