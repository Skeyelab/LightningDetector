async function fetchStatus() {
    try {
        const res = await fetch('/api/v1/status');
        const data = await res.json();
        document.getElementById('status-json').textContent = JSON.stringify(data, null, 2);
    } catch (err) {
        document.getElementById('status-json').textContent = 'Error fetching status';
    }
}

async function fetchConfig() {
    try {
        const res = await fetch('/api/v1/config');
        const cfg = await res.json();
        if (typeof cfg.lora_preset !== 'undefined') {
            document.getElementById('loraPresetSelect').value = cfg.lora_preset;
            updatePresetInfo(cfg.lora_preset);
        }
    } catch (err) {
        console.error('Failed to fetch config', err);
    }
}

function updatePresetInfo(presetIndex) {
    const presets = [
        { name: "Long Range - Fast", bw: "125kHz", sf: "SF10" },
        { name: "Long Range - Slow", bw: "125kHz", sf: "SF12" },
        { name: "Long Range - Moderate", bw: "125kHz", sf: "SF11" },
        { name: "Medium Range - Slow", bw: "125kHz", sf: "SF10" },
        { name: "Medium Range - Fast", bw: "250kHz", sf: "SF9" },
        { name: "Short Range - Slow", bw: "125kHz", sf: "SF8" },
        { name: "Short Range - Fast", bw: "250kHz", sf: "SF7" },
        { name: "Short Range - Turbo", bw: "500kHz", sf: "SF7" }
    ];

    if (presetIndex >= 0 && presetIndex < presets.length) {
        const preset = presets[presetIndex];
        document.getElementById('current-preset').textContent = preset.name;
        document.getElementById('current-bw').textContent = preset.bw;
        document.getElementById('current-sf').textContent = preset.sf;
    }
}

async function saveConfig() {
    const preset = parseInt(document.getElementById('loraPresetSelect').value, 10);
    updatePresetInfo(preset);

    try {
        const response = await fetch('/api/v1/preset', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ preset: preset })
        });

        if (response.ok) {
            console.log('Preset saved successfully:', preset);
            // Show success feedback
            const statusDiv = document.createElement('div');
            statusDiv.textContent = 'Preset saved!';
            statusDiv.style.cssText = 'color: green; margin-top: 10px; font-weight: bold;';
            document.getElementById('lora-config').appendChild(statusDiv);

            // Remove the status after 3 seconds
            setTimeout(() => {
                if (statusDiv.parentNode) {
                    statusDiv.parentNode.removeChild(statusDiv);
                }
            }, 3000);
        } else {
            console.error('Failed to save preset:', response.status);
            alert('Failed to save preset. Check console for details.');
        }
    } catch (err) {
        console.error('Error saving preset:', err);
        alert('Error saving preset: ' + err.message);
    }
}

window.addEventListener('load', () => {
    fetchStatus();
    fetchConfig();
    setInterval(fetchStatus, 5000);
    setInterval(fetchConfig, 3000); // Poll for config changes every 3 seconds
    document.getElementById('loraPresetSelect').addEventListener('change', saveConfig);
});
