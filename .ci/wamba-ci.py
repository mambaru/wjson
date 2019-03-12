#!/usr/bin/python3
# -*- coding: utf-8 -*-
# coding: UTF-8

import sys
import re
import json
import io
sys.path.insert(0, '.')

def get_key():
  if len(sys.argv) > 2:
    return sys.argv[2]
  else: 
    return None

def get_def():
  if len(sys.argv) > 3 : 
    return sys.argv[3]
  else: 
    return None

def get_file():
  if len(sys.argv) > 4: 
    return sys.argv[4]
  else: 
    return "wamba-ci.json"

def load_json():
  fname = get_file()
  with open(fname) as f:
    text=f.read()
  sjson = re.sub('//.*?\n|/\*.*?\*/', '', text, flags=re.S)
  return json.loads(sjson)

def get_val():
  ojson = load_json()
  obj = ojson;
  keystr = get_key() 
  if keystr is None:
    return None
  keys = keystr.split('.')
  for k in keys:
    if k in obj:
      obj=obj[k]
    else:
      return None 
  return obj
  
def get():
  val = get_val()
  if val != None:
    return val
  return get_def()
  
def has():
  val = get_val()
  if not val is None:
    return "ON"
  else:
    return "OFF"

# return
#   ON  если ключ существует, и значение true
#   OFF во всех остальных случаях 
def option():
  val = get_val()
  
  if val == None:
    defv=get_def()
    if defv==None:
      return "OFF"
    elif defv==True or defv=="ON":
      return defv
    else:
      return "OFF"
    
  if val==True or val=="ON":
    return "ON"
  return "OFF"
  

if __name__ == '__main__':
  if len(sys.argv) > 1 :
    cmd=sys.argv[1]
  else:
    cmd="Empty"
    
  if cmd=="get":
    res = get()
  elif cmd=="has":
    res = has()
  elif cmd=="option":
    res = option()
  else:
    print("Error : Unknown command: " + cmd)
    exit(1)
    
  print(res)

    
  
  
    
