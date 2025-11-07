import 'package:flutter/material.dart';
import 'plant_info_card.dart';
import 'add_device_page.dart';

/// The main screen of the app, displaying a welcome message and a list of plants.
class HomePage extends StatefulWidget {
  final String username;

  /// Creates a [HomePage].
  ///
  /// The [username] is required to display the welcome message.
  const HomePage({super.key, required this.username});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  // A map to hold the on/off state of each device.
  final Map<String, bool> _deviceStatus = {
    'Monstera': true,
    'Ficus Lyrata': false,
  };

  void _updateDeviceStatus(String plantName, bool newStatus) {
    setState(() {
      _deviceStatus[plantName] = newStatus;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('FloRa Home'),
        backgroundColor: Colors.transparent,
        elevation: 0,
        foregroundColor: Colors.white,
      ),
      extendBodyBehindAppBar: true,
      body: Stack(
        fit: StackFit.expand,
        children: [
          // Background image
          Image.asset(
            'assets/FloRa_background.jpg',
            fit: BoxFit.cover,
            // Apply a color filter to make the image less prominent.
            color: Colors.white.withOpacity(0.5),
            colorBlendMode: BlendMode.darken,
          ),
          ListView(
            children: [
              // Welcome message
              Padding(
                padding: const EdgeInsets.fromLTRB(
                    16.0, kToolbarHeight + 16.0, 16.0, 16.0),
                child: Text(
                  'Hello ${widget.username},',
                  style: Theme.of(context)
                      .textTheme
                      .headlineSmall
                      ?.copyWith(color: Colors.white),
                ),
              ),

              // List of plant cards
              PlantInfoCard(
                plantName: 'Monstera',
                temperature: 23.5,
                humidity1: 65,
                humidity2: 70,
                light: 800,
                battery: 85,
                isDeviceOn: _deviceStatus['Monstera'] ?? true,
                onStatusChanged: (newStatus) {
                  _updateDeviceStatus('Monstera', newStatus);
                },
              ),
              PlantInfoCard(
                plantName: 'Ficus Lyrata',
                temperature: 22.1,
                humidity1: 55,
                humidity2: 60,
                light: 1200,
                battery: 92,
                isDeviceOn: _deviceStatus['Ficus Lyrata'] ?? false,
                onStatusChanged: (newStatus) {
                  _updateDeviceStatus('Ficus Lyrata', newStatus);
                },
              ),
            ],
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          Navigator.of(context).push(
            MaterialPageRoute(builder: (_) => const AddDevicePage()),
          );
        },
        backgroundColor: Theme.of(context).colorScheme.primary,
        child: const Icon(Icons.add, color: Colors.white),
      ),
    );
  }
}
