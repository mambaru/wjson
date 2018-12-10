#!/usr/bin/python3
# -*- coding: utf-8 -*-
# coding: UTF-8

import sys
import re
import json
import io
sys.path.insert(0, '.')

def load_json(fname):
  with open(fname) as f:
    text=f.read()
  # Удаление комментариев в си стиле
  sjson = re.sub('//.*?\n|/\*.*?\*/', '', text, flags=re.S)
  return json.loads(sjson)

# Получить значение переменной из json файла
def get():
  if len(sys.argv) > 2 : keystr=sys.argv[2]
  else: 
    print("ERROR(wamba-ci.py get): key requred ")
    exit(1)
    
  if len(sys.argv) > 3 : defval=sys.argv[3]
  else: 
    print("ERROR(wamba-ci.py get): default value requred ")
    exit(1)
    
  if len(sys.argv) > 4 : fname=sys.argv[4]
  else: fname="wamba-ci.json"
  
  ojson = load_json(fname)
  curobj=ojson
  key=keystr.split('.')
  for k in key:
    if k in curobj:
      curobj=curobj[k]
    else:
      print(defval)
      return 
  if type(curobj) is dict:
    print(defval)
  else:
    print(curobj)

def has():
  if len(sys.argv) > 2:
    keystr=sys.argv[2]
  else: 
    print("ERROR(wamba-ci.py get): key requred ")
    exit(1)

  if len(sys.argv) > 3 : fname=sys.argv[3]
  else: fname="wamba-ci.json"

  ojson = load_json(fname)
  curobj=ojson
  key=keystr.split('.')
  for k in key:
    if k in curobj:
      curobj=curobj[k]
    else:
      print("0")
      return 
  print("1")

def enabled():
  if len(sys.argv) > 2 : keystr=sys.argv[2]
  else: exit(1)
  if len(sys.argv) > 3 : fname=sys.argv[3]
  else: fname="wamba-ci.json"

  ojson = load_json(fname)
  curobj=ojson
  key=keystr.split('.')
  for k in key:
    if k in curobj:
      curobj=curobj[k]
    else:
      print("0")
      return
  if type(curobj) is bool:
    if curobj: print("1")
    else: print("0")
  else:
    print("0")
  
#  if field in ojson: print ojson[field]
#  else: print defval

if __name__ == '__main__':
  if len(sys.argv) > 1 :
    cmd=sys.argv[1]
  if cmd=="get":
    get()
  elif cmd=="has":
    has()
  elif cmd=="enabled":
    enabled()
  else:
    print("Error cmake-ci: Unknown command")
    exit(1)

    
  
  
    
