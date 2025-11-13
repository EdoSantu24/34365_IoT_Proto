import 'package:flutter/material.dart';
import 'settings_page.dart';

/// A card widget to display information about a plant.
class PlantInfoCard extends StatelessWidget {
  final String plantName;
  final double temperature;
  final double humidity1;
  final int humidity2;
  final int light;
  final int battery;
  final bool isDeviceOn;
  final ValueChanged<bool> onStatusChanged;
  final VoidCallback? onReload;

  /// Creates a [PlantInfoCard].
  const PlantInfoCard({
    super.key,
    required this.plantName,
    required this.temperature,
    required this.humidity1,
    required this.humidity2,
    required this.light,
    required this.battery,
    required this.isDeviceOn,
    required this.onStatusChanged,
    this.onReload,
  });

  @override
  Widget build(BuildContext context) {
    return Card(
      margin: const EdgeInsets.symmetric(vertical: 8, horizontal: 16),
      color: Colors.white.withOpacity(0.1),
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Plant name and status
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text(
                  plantName,
                  style: const TextStyle(
                      fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white),
                ),
                Container(
                  padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                  decoration: BoxDecoration(
                    color: isDeviceOn ? Colors.green.shade700 : Colors.red.shade700,
                    borderRadius: BorderRadius.circular(12),
                  ),
                  child: Text(
                    isDeviceOn ? 'ON' : 'OFF',
                    style: const TextStyle(color: Colors.white, fontWeight: FontWeight.bold),
                  ),
                ),
              ],
            ),
            const SizedBox(height: 16),

            // Sensor readings (row 1)
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                _buildInfoColumn('Temperature', '$temperature ºC'),
                _buildInfoColumn('Humidity 1', '${humidity1.toStringAsFixed(1)} %'),
                _buildInfoColumn('Humidity 2', '$humidity2 %'),
              ],
            ),
            const SizedBox(height: 16),

            // Sensor readings (row 2)
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                _buildInfoColumn('Light', '$light lux'),
                _buildInfoColumn('Battery', '$battery %'),
              ],
            ),
            const Divider(height: 32, color: Colors.white54),

            // Action buttons
            Row(
              children: [
                IconButton(
                  icon: const Icon(Icons.refresh, color: Colors.white),
                  onPressed: onReload,
                  tooltip: 'Reload',
                ),
                IconButton(
                  icon: const Icon(Icons.settings, color: Colors.white),
                  onPressed: () async {
                    final newStatus = await Navigator.of(context).push<bool>(
                      MaterialPageRoute(
                        builder: (_) => SettingsPage(isDeviceOn: isDeviceOn),
                      ),
                    );
                    if (newStatus != null) {
                      onStatusChanged(newStatus);
                    }
                  },
                  tooltip: 'Settings',
                ),
              ],
            )
          ],
        ),
      ),
    );
  }

  /// Builds a small column to display a sensor reading.
  Widget _buildInfoColumn(String title, String value) {
    return Column(
      children: [
        Text(title, style: const TextStyle(color: Colors.white70)),
        const SizedBox(height: 4),
        Text(value,
            style: const TextStyle(
                fontWeight: FontWeight.w600, color: Colors.white)),
      ],
    );
  }
}
