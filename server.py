from flask import Flask, render_template, request
import json, hashlib

app = Flask(__name__)

### Route function for "/hello_world" ###
@app.route('/hello_world', methods=['GET'])
def route_hello_world():
    return json.dumps({'message': 'hello_world'})

### Route function for "/hash" ###
@app.route('/hash', methods=['GET'])
def route_hash():
    name = request.args.get("name")
    HASHED_VALUE = hashlib.sha256(name.encode()).hexdigest()
    return json.dumps({'result': HASHED_VALUE})

### Route function for "/collatz" ###
@app.route('/collatz', methods=['POST'])
def route_collatz():
    # Receive data from the client
    name = request.form.get("name")
    new_hash = request.form.get("hash")
    number = request.form.get("number")

    # Handle error E1: name and hash value do not match
    HASH_RES = hashlib.sha256(name.encode()).hexdigest()
    if HASH_RES != new_hash:
        return json.dumps({'error': 'HASH NOT MATCHED'})

    # Handle error E2:
    if not number.isdigit():
        return json.dumps({'error': 'NUMBER NOT INTEGER'})
    
    new_number = int(number)

    if new_number % 2 == 1:
        NEXT_COLLATZ_NUMBER = 3 * new_number + 1
    else:
        NEXT_COLLATZ_NUMBER = new_number / 2

    return json.dumps({'result': int(NEXT_COLLATZ_NUMBER)})

if __name__ == '__main__':
    app.run(debug=True)