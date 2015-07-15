Based on https://gist.github.com/citrin/4202877

Usuage: 

a.out filename

Take a newline seperated list of IP addresses and convert them to the appropiate CIDR prefix. 

Example:

./a.out example.txt

~~~
10.10.10.0/26
10.10.10.64/32
10.10.10.67/32
10.10.10.68/30
10.10.10.72/29
10.10.10.80/28
10.10.10.96/27
10.10.10.128/32
~~~
