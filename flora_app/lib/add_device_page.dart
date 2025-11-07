import 'package:flutter/material.dart';

/// A page to add a new device.
class AddDevicePage extends StatelessWidget {
  /// Creates an [AddDevicePage].
  const AddDevicePage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Add Device')),
      body: const Center(
        child: Text('Here you can add a new device.'),
      ),
    );
  }
}
