#!/usr/bin/ruby

require 'rubygems'
require 'socket'
require 'mysql'

httpd = TCPServer.open('0.0.0.0',6000)
loop {
	client = httpd.accept
	client.puts "OK\n"	
	
	line= client.gets
	puts line
	

}

