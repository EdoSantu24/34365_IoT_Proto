
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
      headers: {
        'Authorization': _apiToken,
      },
    );

    if (response.statusCode == 200) {
      print('Response body for $telemetryName: ${response.body}');
      final body = jsonDecode(response.body);

      // The error indicates the response is simpler than expected, like: {"value": 35}
      // This code now handles that simple format directly.
      if (body is Map && body.containsKey('value')) {
        final telemetryValue = body['value'];
        return {telemetryName: telemetryValue};
        print("Data returned");
      } else {
        throw Exception('Telemetry data for "$telemetryName" has an unexpected format. Body: $body');
      }
    } else {
      print('Request failed with status: ${response.statusCode}');
      print('Response body: ${response.body}');
      throw Exception('Failed to load telemetry for "$telemetryName"');
    }
  }
}
