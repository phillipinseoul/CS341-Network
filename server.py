from flask import Flask, render_template, request
import json, hashlib

app = Flask(__name__)

@app.route('/hello_world', methods=['GET'])
def route_hello_world():
    return json.dumps({'message': 'hello_world'})

@app.route('/hash', methods=['GET'])
def route_hash():
    name = request.args.get('name')
    HASHED_VALUE = hashlib.sha256(name.encode()).hexdigest()
    return json.dumps({'result': f'{HASHED_VALUE}'})

@app.route('/collatz', methods=['POST'])
def route_collatz():
    return json.dumps({'result': 'NEXT_COLLATZ_NUMBER'})


if __name__ == '__main__':
    app.run()