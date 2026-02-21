import React from 'react';
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/Card';
import { Badge } from '@/components/ui/Badge';
import { NODE_CONFIG, getPollutionLevel, formatTime, isWithinMinutes } from '@/lib/utils';

export default function NodeCard({ nodeId, data }) {
    const config = NODE_CONFIG[nodeId];

    // Checks if node is online (data received in last 5 minutes)
    const isOnline = data && isWithinMinutes(data.timestamp, 5);

    // Extract sensor values and CONVERT TO NUMBERS (in case they're strings from CSV)
    const temperature = data?.temperature != null ? Number(data.temperature) : undefined;
    const humidity = data?.humidity != null ? Number(data.humidity) : undefined;
    const no2 = data?.no2 != null ? Number(data.no2) : undefined;
    const pm25 = data?.pm25 != null ? Number(data.pm25) : undefined;
    const co = data?.co != null ? Number(data.co) : undefined;
    const o3 = data?.o3 != null ? Number(data.o3) : undefined;
    const nh3 = data?.nh3 != null ? Number(data.nh3) : undefined;

    // Get pollution levels
    const no2Level = no2 != null ? getPollutionLevel('no2', no2) : null;
    const pm25Level = pm25 != null ? getPollutionLevel('pm25', pm25) : null;

    const lastUpdate = data?.timestamp ? formatTime(data.timestamp) : 'No data';

    // Colors to differentiate each node
    const colorClasses = {
        blue: 'from-blue-500 to-blue-600',
        emerald: 'from-emerald-500 to-emerald-600',
        orange: 'from-orange-500 to-orange-600'
    };

    return (
        <Card className="overflow-hidden">
            {/* Header with gradient */}
            <div className={`bg-gradient-to-r ${colorClasses[config.color]} p-4 text-white`}>
                <div className="flex items-start justify-between">
                    <div className="flex items-center gap-3">
                        <div className="text-3xl">{config.emoji}</div>
                        <div>
                            <h3 className="font-bold text-lg">{nodeId}: {config.name}</h3>
                            <p className="text-white/80 text-sm">{config.description}</p>
                        </div>
                    </div>
                    <Badge variant={isOnline ? 'success' : 'danger'} className="bg-white/20 text-white border-0">
                        <span className={`w-2 h-2 rounded-full mr-1.5 ${isOnline ? 'bg-green-400 animate-pulse' : 'bg-red-300'}`} />
                        {isOnline ? 'ONLINE' : 'OFFLINE'}
                    </Badge>
                </div>
                <p className="text-white/70 text-xs mt-3">Updated {lastUpdate}</p>
            </div>

            {/* Metrics Grid */}
            <CardContent className="p-4 grid grid-cols-2 gap-3">
                {/* Temperature */}
                <MetricBox
                    label="Temperature"
                    value={temperature !== undefined && !isNaN(temperature) ? temperature.toFixed(1) : 'N/A'}
                    unit="°C"
                />

                {/* Humidity */}
                <MetricBox
                    label="Humidity"
                    value={humidity !== undefined && !isNaN(humidity) ? humidity.toFixed(0) : 'N/A'}
                    unit="%"
                />

                {/* NO₂ */}
                <MetricBox
                    label="NO₂"
                    value={no2 !== undefined && !isNaN(no2) ? no2.toFixed(1) : 'N/A'}
                    unit="µg/m³"
                    level={no2Level}
                />

                {/* PM2.5 */}
                <MetricBox
                    label="PM2.5"
                    value={pm25 !== undefined && !isNaN(pm25) ? pm25.toFixed(1) : 'N/A'}
                    unit="µg/m³"
                    level={pm25Level}
                />

                {/* CO */}
                <MetricBox
                    label="CO"
                    value={co !== undefined && !isNaN(co) ? co.toFixed(1) : 'N/A'}
                    unit="ppm"
                />

                {/* O₃ */}
                <MetricBox
                    label="O₃"
                    value={o3 !== undefined && !isNaN(o3) ? o3.toFixed(1) : 'N/A'}
                    unit="ppb"
                />

                {/* NH₃ */}
                <MetricBox
                    label="NH₃"
                    value={nh3 !== undefined && !isNaN(nh3) ? nh3.toFixed(2) : 'N/A'}
                    unit="V"
                />
            </CardContent>
        </Card>
    );
}

function MetricBox({ label, value, unit, level }) {
    return (
        <div className={`p-3 rounded-xl border ${level ? level.bgColor : 'bg-gray-50 border-gray-100'}`}>
            <div className="text-xs font-medium opacity-70 mb-1">{label}</div>
            <div className="flex items-baseline gap-1">
                <span className="text-2xl font-bold">{value}</span>
                <span className="text-xs opacity-60">{unit}</span>
            </div>
            {level && (
                <div className={`text-xs font-medium mt-1 ${level.color}`}>
                    {level.level}
                </div>
            )}
        </div>
    );
}