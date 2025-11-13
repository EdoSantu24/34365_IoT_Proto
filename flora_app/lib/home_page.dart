import 'package:flutter/material.dart';
import 'plant_info_card.dart';
import 'add_device_page.dart';
import 'azure_iot_service.dart';

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
  final AzureIoTService _azureIoTService = AzureIoTService();

  // State for each plant
  final Map<String, bool> _deviceStatus = {
    'Monstera': true,
    'Ficus Lyrata': false,
  };

  final Map<String, double> _temperatures = {
    'Monstera': 23.5,
    'Ficus Lyrata': 22.1,
  };

  final Map<String, double> _humidities = {
    'Monstera': 65.0,
    'Ficus Lyrata': 55.0,
  };

  final Map<String, double> _soilHumidities = {
    'Monstera': 70.0,
    'Ficus Lyrata': 60.0,
  };

  final Map<String, double> _lights = {
    'Monstera': 800.0,
    'Ficus Lyrata': 1200.0,
  };

  final Map<String, double> _bat = {
    'Monstera': 80.0,
    'Ficus Lyrata': 20.0,
  };


  void _updateDeviceStatus(String plantName, bool newStatus) {
    setState(() {
      _deviceStatus[plantName] = newStatus;
    });
  }

  Future<void> _updatePlantData(String plantName, String deviceId) async {
    print('Updating data for $plantName from device $deviceId...');
    try {
      // Fetch all telemetry data in parallel
      final results = await Future.wait([
        _azureIoTService.getLatestTelemetry(deviceId, 'Temp'),
        _azureIoTService.getLatestTelemetry(deviceId, 'Hum'),
        _azureIoTService.getLatestTelemetry(deviceId, 'Soil'),
        _azureIoTService.getLatestTelemetry(deviceId, 'Light'),
        _azureIoTService.getLatestTelemetry(deviceId, 'Bat'),
      ]);

      final tempResult = results[0];
      final humResult = results[1];
      final soilResult = results[2];
      final lightResult = results[3];
      final batResult = results[4];

      print('Received data for $plantName: $tempResult, $humResult, $soilResult, $lightResult, $batResult');

      setState(() {
        // Update state with the new values
        if (tempResult.containsKey('Temp')) {
          _temperatures[plantName] = (tempResult['Temp'] as num).toDouble();
        }
        if (humResult.containsKey('Hum')) {
          _humidities[plantName] = (humResult['Hum'] as num).toDouble();
        }
        if (soilResult.containsKey('Soil')) {
          _soilHumidities[plantName] = (soilResult['Soil'] as num).toDouble();
        }
        if (lightResult.containsKey('Light')) {
          _lights[plantName] = (lightResult['Light'] as num).toDouble();
        }
        if (batResult.containsKey('Bat')) {
          _bat[plantName] = (batResult['Bat'] as num).toDouble();
        }
      });
    } catch (e) {
      print('Error fetching data for $plantName: $e');
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Error fetching data for $plantName: $e'),
            backgroundColor: Colors.red,
          ),
        );
      }
    }
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
                temperature: _temperatures['Monstera']!,
                humidity1: _humidities['Monstera']!,
                humidity2: _soilHumidities['Monstera']!.toInt(),
                light: _lights['Monstera']!.toInt(),
                battery: _bat['Monstera']!.toInt(),
                isDeviceOn: _deviceStatus['Monstera']!,
                onStatusChanged: (newStatus) =>
                    _updateDeviceStatus('Monstera', newStatus),
                onReload: () => _updatePlantData('Monstera', 'plant-sensor-01'),
              ),
              PlantInfoCard(
                plantName: 'Ficus Lyrata',
                temperature: _temperatures['Ficus Lyrata']!,
                humidity1: _humidities['Ficus Lyrata']!,
                humidity2: _soilHumidities['Ficus Lyrata']!.toInt(),
                light: _lights['Ficus Lyrata']!.toInt(),
                battery: _bat['Ficus Lyrata']!.toInt(),
                isDeviceOn: _deviceStatus['Ficus Lyrata']!,
                onStatusChanged: (newStatus) =>
                    _updateDeviceStatus('Ficus Lyrata', newStatus),
                onReload: () => _updatePlantData('Ficus Lyrata', 'plant-sensor-01'),
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
