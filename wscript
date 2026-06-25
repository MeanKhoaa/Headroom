from pebble_sdk import *

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def build(ctx):
    ctx.load('pebble_sdk')
    build_worker = ctx(features='pebble_app', target='pebble-app.elf')
