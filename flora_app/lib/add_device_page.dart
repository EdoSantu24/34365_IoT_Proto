import 'package:flutter/material.dart';
import 'azure_iot_service.dart';

/// A page to add a new device and configure its plant type.
class AddDevicePage extends StatefulWidget {
  final List<String> usedDeviceIds;

  /// Creates an [AddDevicePage].
  const AddDevicePage({super.key, required this.usedDeviceIds});

  @override
  State<AddDevicePage> createState() => _AddDevicePageState();
}

class _AddDevicePageState extends State<AddDevicePage> {
  final AzureIoTService _azureIoTService = AzureIoTService();

  List<String> _deviceIds = [];
  String? _selectedDeviceId;

  // Map for plant types: Name -> ID
  final Map<String, int> _plantTypes = {
    'Cactus': 1,
    'Monstera': 2,
    //Add more
  };
  String? _selectedPlantName;

  bool _isLoading = true; // Start in loading state

  @override
  void initState() {
    super.initState();
    _fetchDevices();
  }

  Future<void> _fetchDevices() async {
    if (!mounted) return;
    setState(() => _isLoading = true);
    try {
      final devices = await _azureIoTService.getDevices();
      
      // Filter devices to only include those starting with 'plant' and not already in use
      final availableDevices = devices
          .where((id) => id.startsWith('plant') && !widget.usedDeviceIds.contains(id))
          .toList();

      if (availableDevices.isEmpty && mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('No new devices available to add.'),
            backgroundColor: Colors.orange,
          ),
        );
      }
      
      if(mounted) {
        setState(() {
          _deviceIds = availableDevices;
          if (availableDevices.isNotEmpty) {
            _selectedDeviceId = availableDevices.first;
          }
        });
      }
    } catch (e) {
      if(mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Error fetching devices: $e'), backgroundColor: Colors.red),
        );
      }
    } finally {
      if(mounted) {
        setState(() => _isLoading = false);
      }
    }
  }

  Future<void> _configureDevice() async {
    if (_selectedDeviceId == null || _selectedPlantName == null) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Please select a device and a plant type.')),
      );
      return;
    }

    setState(() => _isLoading = true);
    try {
      final plantTypeId = _plantTypes[_selectedPlantName]!;
      await _azureIoTService.updateDevicePlantType(_selectedDeviceId!, plantTypeId);

      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Success! configured $_selectedDeviceId as $_selectedPlantName'),
            backgroundColor: Colors.green,
          ),
        );
        // Go back to the home page after successfully adding a device
        Navigator.of(context).pop();
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Error configuring device: $e'), backgroundColor: Colors.red),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isLoading = false);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    // Get the top padding (status bar height) to prevent overlap
    final double topPadding = MediaQuery.of(context).padding.top + kToolbarHeight + 16.0;

    return Scaffold(
      appBar: AppBar(
        title: const Text('Add Device'),
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
          Padding(
            padding: EdgeInsets.fromLTRB(16.0, topPadding, 16.0, 16.0),
            child: _isLoading
                ? const Center(child: CircularProgressIndicator(color: Colors.white))
                : _deviceIds.isEmpty
                    ? const Center(
                        child: Text(
                          'No available devices found.',
                          style: TextStyle(color: Colors.white, fontSize: 18),
                          textAlign: TextAlign.center,
                        ),
                      )
                    : SingleChildScrollView(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.stretch,
                          children: [
                            const Text(
                              'Configure a new device',
                              style: TextStyle(
                                fontSize: 20,
                                fontWeight: FontWeight.bold,
                                color: Colors.white,
                              ),
                            ),
                            const SizedBox(height: 24),

                            // Device Selector
                            DropdownButtonFormField<String>(
                              decoration: const InputDecoration(
                                labelText: 'Select Device ID',
                                labelStyle: TextStyle(color: Colors.white70),
                                enabledBorder: OutlineInputBorder(
                                  borderSide: BorderSide(color: Colors.white70),
                                ),
                                focusedBorder: OutlineInputBorder(
                                  borderSide: BorderSide(color: Colors.white),
                                ),
                                border: OutlineInputBorder(),
                              ),
                              dropdownColor: Colors.grey[800],
                              style: const TextStyle(color: Colors.white),
                              value: _selectedDeviceId,
                              items: _deviceIds.map((id) {
                                return DropdownMenuItem(value: id, child: Text(id));
                              }).toList(),
                              onChanged: (val) => setState(() => _selectedDeviceId = val),
                            ),
                            const SizedBox(height: 16),

                            // Plant Type Selector
                            DropdownButtonFormField<String>(
                              decoration: const InputDecoration(
                                labelText: 'Select Plant Type',
                                labelStyle: TextStyle(color: Colors.white70),
                                enabledBorder: OutlineInputBorder(
                                  borderSide: BorderSide(color: Colors.white70),
                                ),
                                focusedBorder: OutlineInputBorder(
                                  borderSide: BorderSide(color: Colors.white),
                                ),
                                border: OutlineInputBorder(),
                              ),
                              dropdownColor: Colors.grey[800],
                              style: const TextStyle(color: Colors.white),
                              value: _selectedPlantName,
                              items: _plantTypes.keys.map((name) {
                                return DropdownMenuItem(value: name, child: Text(name));
                              }).toList(),
                              onChanged: (val) => setState(() => _selectedPlantName = val),
                            ),
                            const SizedBox(height: 32),

                            // Submit Button
                            ElevatedButton(
                              onPressed: _isLoading ? null : _configureDevice,
                              style: ElevatedButton.styleFrom(
                                padding: const EdgeInsets.symmetric(vertical: 16),
                                backgroundColor: Colors.green.shade700,
                                foregroundColor: Colors.white,
                              ),
                              child: _isLoading
                                  ? const SizedBox(
                                      height: 20,
                                      width: 20,
                                      child: CircularProgressIndicator(strokeWidth: 2, color: Colors.white),
                                    )
                                  : const Text('Link Device to Plant', style: TextStyle(fontWeight: FontWeight.bold)),
                            ),
                          ],
                        ),
                      ),
          ),
        ],
      ),
    );
  }
}
