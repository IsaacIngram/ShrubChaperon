FROM gcr.io/datadoghq/agent:7
COPY conf.yaml /etc/datadog-agent/conf.d/statsd.d
