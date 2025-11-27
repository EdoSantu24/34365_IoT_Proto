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

  // Map associating Plant Name -> Device ID
  // This ensures we know which device belongs to which plant.
  // We start with only one default plant, now Cactus as requested.
  final Map<String, String> _plantDevices = {
    'Cactus': 'plant-sensor-01',
  };

  // State for each plant (Values) - Initialized with Cactus
  final Map<String, bool> _deviceStatus = {
    'Cactus': false,
  };

  final Map<String, double> _temperatures = {
    'Cactus': 0.0,
  };

  final Map<String, double> _humidities = {
    'Cactus': 0.0,
  };

  final Map<String, double> _soilHumidities = {
    'Cactus': 0.0,
  };

  final Map<String, double> _lights = {
    'Cactus': 0.0,
  };

  final Map<String, double> _bat = {
    'Cactus': 0.0,
  };

  // Thresholds: [Min, Max]
  final Map<String, Map<String, List<double>>> _plantThresholds = {
    'Monstera': {
      'Temp': [18.0, 26.0],
      'Hum': [50.0, 80.0],
      'Soil': [25.0, 35.0],
      'Light': [500.0, 1500.0],
    },
    'Cactus': {
      'Temp': [18.0, 32.0],
      'Hum': [20.0, 50.0],
      'Soil': [10.0, 15.0],
      'Light': [2000.0, 5000.0],
    },
  };

  @override
  void initState() {
    super.initState();
    _initializePage();
  }

  Future<void> _initializePage() async {
    // Iterate over all configured plants and fetch their data
    for (var entry in _plantDevices.entries) {
      final plantName = entry.key;
      final deviceId = entry.value;
      
      await _checkDeviceConnection(plantName, deviceId);
      _updatePlantData(plantName, deviceId);
    }
  }

  Future<void> _checkDeviceConnection(String plantName, String deviceId) async {
    try {
      final devices = await _azureIoTService.getDevices();
      final isOnline = devices.contains(deviceId);
      
      if (mounted) {
        setState(() {
          _deviceStatus[plantName] = isOnline;
        });
      }
      
      print('Device $deviceId for plant $plantName connection status: $isOnline');
    } catch (e) {
      print('Error checking device connection: $e');
      if (mounted) {
        setState(() {
          _deviceStatus[plantName] = false;
        });
      }
    }
  }

  void _updateDeviceStatus(String plantName, bool newStatus) {
    setState(() {
      _deviceStatus[plantName] = newStatus;
    });
  }

  /// Updates the local state when a plant type is changed in settings.
  void _handlePlantTypeChanged(String oldName, String newName) {
    if (oldName == newName) return;

    setState(() {
      final deviceId = _plantDevices.remove(oldName);
      if (deviceId != null) {
        _plantDevices[newName] = deviceId;
      }

      if (_temperatures.containsKey(oldName)) {
        _temperatures[newName] = _temperatures.remove(oldName)!;
      }
      if (_humidities.containsKey(oldName)) {
        _humidities[newName] = _humidities.remove(oldName)!;
      }
      if (_soilHumidities.containsKey(oldName)) {
        _soilHumidities[newName] = _soilHumidities.remove(oldName)!;
      }
      if (_lights.containsKey(oldName)) {
        _lights[newName] = _lights.remove(oldName)!;
      }
      if (_bat.containsKey(oldName)) {
        _bat[newName] = _bat.remove(oldName)!;
      }
      if (_deviceStatus.containsKey(oldName)) {
        _deviceStatus[newName] = _deviceStatus.remove(oldName)!;
      }
    });

    _checkThresholds(newName);
  }

  void _checkThresholds(String plantName) {
    Map<String, List<double>> thresholds;
    
    if (_plantThresholds.containsKey(plantName)) {
      thresholds = _plantThresholds[plantName]!;
    } else {
      // Default thresholds if not specified (Generic Plant)
      thresholds = {
        'Temp': [15.0, 30.0],
        'Hum': [30.0, 70.0],
        'Soil': [20.0, 60.0],
        'Light': [1000.0, 3000.0],
      };
    }

    List<Widget> alertWidgets = [];

    // Check values
    void checkValue(String type, double value, String name, String unit) {
      if (!thresholds.containsKey(type)) return;
      final min = thresholds[type]![0];
      final max = thresholds[type]![1];
      
      String message = '';
      if (value < min) {
        message = 'needs more $name.';
      } else if (value > max) {
        message = 'needs less $name.';
      }

      if (message.isNotEmpty) {
        alertWidgets.add(
          Padding(
            padding: const EdgeInsets.only(bottom: 12.0),
            child: Text.rich(
              TextSpan(
                style: const TextStyle(fontSize: 18, color: Colors.black87),
                children: [
                  const TextSpan(text: '• '),
                  TextSpan(
                    text: plantName,
                    style: const TextStyle(fontWeight: FontWeight.bold),
                  ),
                  TextSpan(text: ': $message'),
                ],
              ),
            ),
          ),
        );
      }
    }

    // Only check if we have data
    if (_temperatures[plantName] != 0.0) checkValue('Temp', _temperatures[plantName]!, 'temperature', 'ºC');
    if (_humidities[plantName] != 0.0) checkValue('Hum', _humidities[plantName]!, 'humidity in the atmosphere', '%');
    if (_soilHumidities[plantName] != 0.0) checkValue('Soil', _soilHumidities[plantName]!, 'water', '%');
    if (_lights[plantName] != 0.0) checkValue('Light', _lights[plantName]!, 'light', 'lux');

    if (alertWidgets.isNotEmpty) {
      showDialog(
        context: context,
        builder: (context) => AlertDialog(
          title: Text('Alerts for $plantName', style: const TextStyle(fontSize: 22, fontWeight: FontWeight.bold)),
          content: SingleChildScrollView(
            child: Column(
              mainAxisSize: MainAxisSize.min,
              crossAxisAlignment: CrossAxisAlignment.start,
              children: alertWidgets,
            ),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text('OK', style: TextStyle(fontSize: 18)),
            ),
          ],
        ),
      );
    }
  }

  Future<void> _updatePlantData(String plantName, String deviceId) async {
    print('Updating data for $plantName from device $deviceId...');
    try {
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

      if (mounted) {
        setState(() {
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
          
          _deviceStatus[plantName] = true;
        });

        // Check thresholds after update
        _checkThresholds(plantName);
      }

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
          Image.asset(
            'assets/FloRa_background.jpg',
            fit: BoxFit.cover,
            color: Colors.white.withOpacity(0.5),
            colorBlendMode: BlendMode.darken,
          ),
          ListView(
            children: [
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

              // Dynamically build the list of PlantInfoCards
              ..._plantDevices.entries.map((entry) {
                final String plantName = entry.key;
                final String deviceId = entry.value;

                return PlantInfoCard(
                  plantName: plantName,
                  deviceId: deviceId, // Pass device ID to card
                  temperature: _temperatures[plantName] ?? 0.0,
                  humidity1: _humidities[plantName] ?? 0.0,
                  humidity2: (_soilHumidities[plantName] ?? 0.0).toInt(),
                  light: (_lights[plantName] ?? 0.0).toInt(),
                  battery: (_bat[plantName] ?? 0.0).toInt(),
                  isDeviceOn: _deviceStatus[plantName] ?? false,
                  onStatusChanged: (newStatus) =>
                      _updateDeviceStatus(plantName, newStatus),
                  onPlantTypeChanged: (newPlantName) => 
                      _handlePlantTypeChanged(plantName, newPlantName),
                  onReload: () => _updatePlantData(plantName, deviceId),
                );
              }),
            ],
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          // Get the list of all currently used device IDs
          final usedIds = _plantDevices.values.toList();
          
          Navigator.of(context).push(
            MaterialPageRoute(
              builder: (_) => AddDevicePage(usedDeviceIds: usedIds),
            ),
          );
        },
        backgroundColor: Theme.of(context).colorScheme.primary,
        child: const Icon(Icons.add, color: Colors.white),
      ),
    );
  }
}
