import 'dart:convert';
import 'package:http/http.dart' as http;

class AzureIoTService {
  static const String _subdomain = '34365-25';
  static const String _apiToken =
      'SharedAccessSignature sr=a2c2e2c0-feec-4306-b4b9-f440fee98971&sig=Dht%2BQYpYJacwu6C6rcUihzsvh43m9XsLgfhj3YY0udU%3D&skn=Flutter-App-Token&se=1794559166643';

  Future<Map<String, dynamic>> getLatestTelemetry(String deviceId, String telemetryName) async {
    final url = Uri.https(
      '$_subdomain.azureiotcentral.com',
      '/api/devices/$deviceId/telemetry/$telemetryName',
      {'api-version': '2022-07-31'},
    );
    print('Requesting URL: $url');

    final response = await http.get(
      url,
      headers: {'Authorization': _apiToken},
    );

    if (response.statusCode == 200) {
      final body = jsonDecode(response.body);

      if (body is Map && body.containsKey('value')) {
        final dynamic valueField = body['value'];

        if (valueField is List && valueField.isNotEmpty) {
          final dynamic firstElement = valueField[0];
          if (firstElement is Map && firstElement.containsKey('value')) {
            return {telemetryName: firstElement['value']};
          }
        } else if (valueField is num) {
          return {telemetryName: valueField};
        }
      }
      throw Exception('Telemetry data for "$telemetryName" has an unexpected format. Body: $body');
    } else {
      throw Exception('Failed to load telemetry for "$telemetryName"');
    }
  }

  /// Retrive a list of all device IDs from Azure IoT Central.
  Future<List<String>> getDevices() async {
    final url = Uri.https(
      '$_subdomain.azureiotcentral.com',
      '/api/devices',
      {'api-version': '2022-07-31'},
    );
    print('Requesting URL: $url');
    final response = await http.get(
      url,
      headers: {'Authorization': _apiToken},
    );

    if (response.statusCode == 200) {
      final body = jsonDecode(response.body);
      if (body['value'] != null && body['value'] is List) {
        final List<String> deviceIds = (body['value'] as List)
            .map<String>((device) => device['id'] as String)
            .toList();
        print('Found devices: $deviceIds');
        return deviceIds;
      }
      return [];
    } else {
      throw Exception('Failed to get devices. Status: ${response.statusCode}, Body: ${response.body}');
    }
  }

  /// Updates the 'Type' property for a specific device.
  Future<void> updateDevicePlantType(String deviceId, int plantType) async {
    final url = Uri.https(
      '$_subdomain.azureiotcentral.com',
      '/api/devices/$deviceId/properties',
      {'api-version': '2022-07-31'},
    );
    print('Requesting URL: $url');

    try {
      // Changed POST to PUT as per Azure IoT Central API standards for properties
      final response = await http.patch(
        url,
        headers: {
          'Authorization': _apiToken,
          'Content-Type': 'application/json',
        },
        body: jsonEncode({'Type': plantType}),
      );
      print('Response status code: ${response.statusCode}');
      print('Response body: ${response.body}');

      if (response.statusCode >= 200 && response.statusCode < 300) {
        print('Successfully sent Type update for device $deviceId.');
      } else {
        throw Exception(
            'Failed to update plant type. Status: ${response.statusCode}, Body: ${response.body}');
      }
    } catch (e) {
      print('Error updating plant type: $e');
      rethrow; // Allow the UI to handle the error
    }
  }
}
