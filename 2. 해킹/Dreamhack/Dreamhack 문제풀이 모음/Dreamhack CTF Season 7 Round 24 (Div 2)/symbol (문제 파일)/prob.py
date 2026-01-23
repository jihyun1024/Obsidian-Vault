#!/usr/bin/env python3
from os import urandom
from random import randrange
from Crypto.Util.number import getPrime

BITS = 64
token = urandom(BITS//8)
token = int.from_bytes(token,"big")

p = getPrime(512)
while p%4 == 1:
    p = getPrime(512)
print(f"{p = }")

for i in range(BITS):
    bit = (token>>i)&1
    k = randrange(2,p-1)**2%p
    if bit:
        k = -k%p
    print(k)

answer = int(input("> "))
if answer == token:
    print(open("flag","r").read())
else:
    print("Wrong!")