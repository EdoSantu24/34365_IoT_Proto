import 'package:flutter/material.dart';

void main() {
  //This function runs the APP
  runApp(MaterialApp(
    home: Scaffold(
      appBar: AppBar(
        title: Text('FloRa App'),
        centerTitle: true,
      ),
      body: Center(
          child: Text("hello Gea")
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {},
        child: Text("Click"),
      ),
    ),
  ));
}