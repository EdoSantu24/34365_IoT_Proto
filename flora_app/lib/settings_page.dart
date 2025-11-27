import 'package:flutter/material.dart';
import 'azure_iot_service.dart';

/// A page to configure plant settings.
class SettingsPage extends StatefulWidget {
  final String deviceId;
  final String currentPlantName;

  /// Creates a [SettingsPage].
  const SettingsPage({
    super.key,
    required this.deviceId,
    required this.currentPlantName,
  });

  @override
  State<SettingsPage> createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  final AzureIoTService _azureIoTService = AzureIoTService();
  
  // Map for plant types: Name -> ID
  final Map<String, int> _plantTypes = {
    'Cactus': 0,
    'Monstera': 1,
  };
  String? _selectedPlantName;
  bool _isLoading = false;

  @override
  void initState() {
    super.initState();
    // Try to set the initial selection based on the passed plant name
    if (_plantTypes.containsKey(widget.currentPlantName)) {
      _selectedPlantName = widget.currentPlantName;
    } else {
      _selectedPlantName = _plantTypes.keys.first;
    }
  }

  Future<void> _updateConfiguration() async {
    if (_selectedPlantName == null) return;

    setState(() => _isLoading = true);
    try {
      final plantTypeId = _plantTypes[_selectedPlantName]!;
      await _azureIoTService.updateDevicePlantType(widget.deviceId, plantTypeId);

      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Success! Configured ${widget.deviceId} as $_selectedPlantName'),
            backgroundColor: Colors.green,
          ),
        );
        // Return the new plant name to the previous screen
        Navigator.of(context).pop(_selectedPlantName);
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
    // Fix for overlapping titles: Add status bar height + toolbar height
    final double topPadding = MediaQuery.of(context).padding.top + kToolbarHeight + 16.0;

    return Scaffold(
      appBar: AppBar(
        title: const Text('Settings'),
        backgroundColor: Colors.transparent,
        elevation: 0,
        foregroundColor: Colors.white,
      ),
      extendBodyBehindAppBar: true,
      body: Stack(
        fit: StackFit.expand,
        children: [
          // Background image matching the rest of the app
          Image.asset(
            'assets/FloRa_background.jpg',
            fit: BoxFit.cover,
            color: Colors.white.withOpacity(0.5),
            colorBlendMode: BlendMode.darken,
          ),
          Padding(
            padding: EdgeInsets.fromLTRB(16.0, topPadding, 16.0, 16.0),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Text(
                  'Configure Plant for ${widget.deviceId}',
                  style: const TextStyle(
                    fontSize: 20,
                    fontWeight: FontWeight.bold,
                    color: Colors.white,
                  ),
                ),
                const SizedBox(height: 24),

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

                // Update Button
                ElevatedButton(
                  onPressed: _isLoading ? null : _updateConfiguration,
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
                      : const Text('Update Configuration', style: TextStyle(fontWeight: FontWeight.bold)),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
