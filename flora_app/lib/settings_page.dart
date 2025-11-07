import 'package:flutter/material.dart';

/// A page to configure plant settings.
class SettingsPage extends StatefulWidget {
  final bool isDeviceOn;

  /// Creates a [SettingsPage].
  const SettingsPage({super.key, required this.isDeviceOn});

  @override
  State<SettingsPage> createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  String? _selectedPlantType = 'Monstera';
  late bool _isDeviceOn;

  @override
  void initState() {
    super.initState();
    _isDeviceOn = widget.isDeviceOn;
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      onWillPop: () async {
        // Return the new status when the user presses the back button.
        Navigator.of(context).pop(_isDeviceOn);
        return true;
      },
      child: Scaffold(
        appBar: AppBar(
          title: const Text('Settings'),
          // Ensure the back button also returns the new status.
          leading: IconButton(
            icon: const Icon(Icons.arrow_back),
            onPressed: () => Navigator.of(context).pop(_isDeviceOn),
          ),
        ),
        body: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              // Plant type selector
              Text(
                'Change plant type',
                style: Theme.of(context).textTheme.titleMedium,
              ),
              const SizedBox(height: 8),
              DropdownButton<String>(
                value: _selectedPlantType,
                isExpanded: true,
                items: ['Monstera', 'Ficus Lyrata', 'Pothos', 'Snake Plant']
                    .map((String value) {
                  return DropdownMenuItem<String>(
                    value: value,
                    child: Text(value),
                  );
                }).toList(),
                onChanged: (newValue) {
                  setState(() {
                    _selectedPlantType = newValue;
                  });
                },
              ),
              const SizedBox(height: 24),

              // Device on/off switch
              SwitchListTile(
                title: const Text('Device Status'),
                value: _isDeviceOn,
                onChanged: (newValue) {
                  setState(() {
                    _isDeviceOn = newValue;
                  });
                },
                secondary: Icon(_isDeviceOn ? Icons.power : Icons.power_off),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
