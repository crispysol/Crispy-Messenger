#!/usr/bin/ruby

require 'socket'

httpd = TCPServer.open('0.0.0.0',6000)
loop {
	client = httpd.accept
	client.puts "OK\n"	
	
	line= client.gets
	puts line
	

}

