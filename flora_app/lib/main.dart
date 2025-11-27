import 'package:flutter/material.dart';
import 'login_page.dart';

// This function runs the app.
void main() {
  runApp(const FloRaApp());
}

class FloRaApp extends StatelessWidget {
  /// Creates a [FloRaApp].
  const FloRaApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'FloRa App',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.green),
        useMaterial3: true,
      ),
      home: const LoginPage(),
    );
  }
}
