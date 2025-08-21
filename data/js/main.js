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
        }
    } catch (err) {
        console.error('Failed to fetch config', err);
    }
}

async function saveConfig() {
    const preset = parseInt(document.getElementById('loraPresetSelect').value, 10);
    await fetch('/api/v1/config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ lora_preset: preset })
    });
}

window.addEventListener('load', () => {
    fetchStatus();
    fetchConfig();
    setInterval(fetchStatus, 5000);
    document.getElementById('loraPresetSelect').addEventListener('change', saveConfig);
});