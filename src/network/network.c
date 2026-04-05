#include "../include/network.h"
#include "../include/stdio.h"
#include "../include/task_manager.h"
#include "../include/driver.h"
#include "../include/filesystem.h"
#include "../include/string.h"
#include "../include/memory.h"

static uint8_t mac_addr[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
static uint8_t ip_addr[4] = {192, 168, 1, 100};
static uint8_t gateway[4] = {192, 168, 1, 1};
static uint8_t dns_server[4] = {8, 8, 8, 8};

static net_packet_t packet_buffer[NET_MAX_PACKETS];
static int packet_head = 0, packet_tail = 0;

void net_init(void) {
    printf("[Network] Initializing TCP/IP stack...\n");
    printf("[Network] MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);
    printf("[Network] IP: %d.%d.%d.%d\n",
           ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    printf("[Network] Gateway: %d.%d.%d.%d\n",
           gateway[0], gateway[1], gateway[2], gateway[3]);
    printf("[Network] DNS: %d.%d.%d.%d\n",
           dns_server[0], dns_server[1], dns_server[2], dns_server[3]);

    // Initialize packet buffer
    memory_set(packet_buffer, 0, sizeof(packet_buffer));

    printf("[Network] TCP/IP stack ready\n");
}

void net_set_ip(uint8_t ip[4]) {
    memory_copy(ip_addr, ip, 4);
}

void net_set_mac(uint8_t mac[6]) {
    memory_copy(mac_addr, mac, 6);
}

int net_send_packet(uint8_t *data, uint32_t size) {
    if (size > NET_MTU) return -1;

    // Simulate network send (in real implementation would use NIC driver)
    printf("[Network] Sending packet (%d bytes)\n", size);

    // Echo back for testing
    kmemcpy(packet_buffer[packet_head].data, data, size);
    packet_buffer[packet_head].size = size;
    packet_buffer[packet_head].timestamp = tick_count;

    packet_head = (packet_head + 1) % NET_MAX_PACKETS;

    return 0;
}

int net_receive_packet(uint8_t *buffer, uint32_t *size) {
    if (packet_tail == packet_head) {
        *size = 0;
        return 0; // No packets
    }

    uint32_t packet_size = packet_buffer[packet_tail].size;
    if (packet_size > *size) packet_size = *size;

    memory_copy(buffer, packet_buffer[packet_tail].data, packet_size);
    *size = packet_size;

    packet_tail = (packet_tail + 1) % NET_MAX_PACKETS;

    return 1; // Success
}

// DNS resolution simulation
int dns_resolve(const char *hostname, uint8_t ip[4]) {
    printf("[DNS] Resolving %s...\n", hostname);

    // Simulate DNS lookup
    if (strcmp(hostname, "ua-horizon.org") == 0) {
        ip[0] = 192; ip[1] = 168; ip[2] = 1; ip[3] = 10;
    } else if (strcmp(hostname, "google.com") == 0) {
        ip[0] = 8; ip[1] = 8; ip[2] = 8; ip[3] = 8;
    } else if (strcmp(hostname, "github.com") == 0) {
        ip[0] = 140; ip[1] = 82; ip[2] = 121; ip[3] = 4;
    } else {
        // Random IP for demo
        ip[0] = 192; ip[1] = 168; ip[2] = 1; ip[3] = (uint8_t)(tick_count % 255);
    }

    printf("[DNS] Resolved to %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    task_delay(20); // Simulate network delay

    return 0;
}

// HTTP client
typedef struct {
    int socket;
    uint8_t remote_ip[4];
    uint16_t remote_port;
    int connected;
} http_connection_t;

int http_get(const char *url, char *response, uint32_t max_size) {
    printf("[HTTP] Connecting to %s...\n", url);

    // Parse URL
    char hostname[128];
    char path[256];
    uint16_t port = 80;

    // Simple URL parsing (http://hostname/path)
    if (strncmp(url, "http://", 7) == 0) {
        const char *host_start = url + 7;
        const char *path_start = strchr(host_start, '/');
        if (path_start) {
            kmemcpy(hostname, host_start, path_start - host_start);
            hostname[path_start - host_start] = 0;
            strcpy(path, path_start);
        } else {
            strcpy(hostname, host_start);
            strcpy(path, "/");
        }
    } else {
        strcpy(hostname, url);
        strcpy(path, "/");
    }

    // DNS resolution
    uint8_t server_ip[4];
    if (dns_resolve(hostname, server_ip) != 0) {
        strcpy(response, "DNS resolution failed");
        return -1;
    }

    // Simulate HTTP request
    printf("[HTTP] GET %s HTTP/1.1\n", path);
    printf("[HTTP] Host: %s\n", hostname);

    task_delay(50); // Simulate network delay

    // Generate demo response based on URL
    if (strstr(url, "ua-horizon.org")) {
        strcpy(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><head><title>UA-Horizon OS</title></head><body>"
            "<h1>Welcome to UA-Horizon OS!</h1>"
            "<p>This is the official website of UA-Horizon, a modern operating system.</p>"
            "<h2>Features:</h2>"
            "<ul>"
            "<li>Custom 64-bit kernel</li>"
            "<li>XP-style GUI desktop</li>"
            "<li>File system with 50MB capacity</li>"
            "<li>Task manager with priorities</li>"
            "<li>Network stack with TCP/IP</li>"
            "<li>Built-in applications (calculator, browser, games)</li>"
            "</ul>"
            "<p>Download the latest version: <a href=\"/download\">ISO Image</a></p>"
            "</body></html>"
        );
    } else if (strstr(url, "google.com")) {
        strcpy(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><body>"
            "<div style='text-align:center;margin-top:100px;'>"
            "<img src='/images/google-logo.png' alt='Google'>"
            "<br><br>"
            "<input type='text' size='50' placeholder='Search Google or type a URL'>"
            "<br><br>"
            "<button>Google Search</button> <button>I'm Feeling Lucky</button>"
            "</div></body></html>"
        );
    } else if (strstr(url, "github.com")) {
        strcpy(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><body>"
            "<div class='container'>"
            "<header><h1>GitHub</h1></header>"
            "<nav><a href='/'>Home</a> | <a href='/explore'>Explore</a> | <a href='/search'>Search</a></nav>"
            "<main>"
            "<h2>UA-Horizon OS Repository</h2>"
            "<p>A modern operating system developed for educational purposes.</p>"
            "<div class='stats'>"
            "<span>⭐ 42 stars</span>"
            "<span>🍴 12 forks</span>"
            "<span>💻 C/ASM</span>"
            "</div>"
            "<a href='/download' class='download-btn'>Download ZIP</a>"
            "</main></div></body></html>"
        );
    } else {
        // Generic demo page
        // Simple HTTP response without sprintf
        copy_str(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><title>");
        concat_str(response, hostname);
        concat_str(response, "</title></head><body><h1>Welcome to ");
        concat_str(response, hostname);
        concat_str(response, "</h1><p>This is a demo webpage for ");
        concat_str(response, hostname);
        concat_str(response, ".</p><p>Current time: ");
        // Simple number to string conversion would be needed here
        concat_str(response, " seconds since boot</p><p>UA-Horizon OS is running!</p></body></html>");
    }

    // Truncate if too long
    if (strlen(response) >= max_size) {
        response[max_size - 1] = 0;
    }

    printf("[HTTP] Received %d bytes\n", strlen(response));
    return 0;
}

int http_download(const char *url, const char *filename) {
    printf("[Download] Downloading %s to %s...\n", url, filename);

    char response[8192];
    if (http_get(url, response, sizeof(response)) != 0) {
        printf("[Download] Failed to download\n");
        return -1;
    }

    // Extract content after headers
    char *content_start = strstr(response, "\r\n\r\n");
    if (!content_start) {
        content_start = strstr(response, "\n\n");
    }
    if (content_start) {
        content_start += (strstr(response, "\r\n\r\n") ? 4 : 2);
    } else {
        content_start = response;
    }

    // Save to file
    uint32_t size = strlen(content_start);
    fs_create_file(filename, size);
    fs_write_file(filename, content_start, size);

    printf("[Download] Saved %d bytes to %s\n", size, filename);
    return 0;
}

// TCP Socket simulation
int net_connect(const char *hostname, uint16_t port) {
    printf("[TCP] Connecting to %s:%d...\n", hostname, port);

    uint8_t server_ip[4];
    if (dns_resolve(hostname, server_ip) != 0) {
        return -1;
    }

    task_delay(30); // Simulate connection delay
    printf("[TCP] Connected to %d.%d.%d.%d:%d\n",
           server_ip[0], server_ip[1], server_ip[2], server_ip[3], port);

    return 0; // Return socket handle
}

int net_listen(uint16_t port) {
    printf("[TCP] Listening on port %d...\n", port);
    return 0;
}

int net_accept(int socket) {
    printf("[TCP] Accepting connection on socket %d...\n", socket);
    task_delay(10);
    return socket + 1;
}

int net_read(int socket, uint8_t *buffer, uint32_t size) {
    printf("[TCP] Reading from socket %d...\n", socket);
    // Simulate receiving data
    strcpy((char*)buffer, "Hello from server!\n");
    return strlen((char*)buffer);
}

int net_write(int socket, uint8_t *data, uint32_t size) {
    printf("[TCP] Writing %d bytes to socket %d\n", size, socket);
    printf("[TCP] Data: %.*s\n", size, data);
    return size;
}

void net_close(int socket) {
    printf("[TCP] Closing socket %d\n", socket);
}

// DNS cache for faster lookups
#define DNS_CACHE_SIZE 10
static struct {
    char domain[64];
    uint32_t ip;
} dns_cache[DNS_CACHE_SIZE];
static int dns_cache_count = 0;

void dns_cache_init(void) {
    // Pre-populate with some entries
    strcpy(dns_cache[0].domain, "google.com");
    dns_cache[0].ip = 0xD83ACDC4; // 216.58.205.196
    dns_cache_count = 1;
}

uint32_t dns_lookup_cached(const char *domain) {
    for (int i = 0; i < dns_cache_count; i++) {
        if (strcmp(dns_cache[i].domain, domain) == 0) {
            return dns_cache[i].ip;
        }
    }
    return 0; // Not found
}

// Simple executable loader (simulates EXE support)
int exe_load(const char *filename) {
    printf("[EXE] Loading %s...\n", filename);

    char buffer[1024];
    int size = fs_read_file(filename, buffer, sizeof(buffer));
    if (size <= 0) {
        printf("[EXE] File not found or empty\n");
        return -1;
    }

    printf("[EXE] File size: %d bytes\n", size);

    // Parse simple script format
    if (strncmp(buffer, "#!/ua-horizon\n", 14) == 0) {
        printf("[EXE] Executing UA-Horizon script...\n");

        // Simple script interpreter
        char *line = buffer + 14;
        while (*line) {
            if (strncmp(line, "print ", 6) == 0) {
                printf("%s", line + 6);
            } else if (strncmp(line, "delay ", 6) == 0) {
                int ms = string_to_int(line + 6);
                task_delay(ms);
            } else if (strncmp(line, "run ", 4) == 0) {
                char app_name[32];
                sscanf(line + 4, "%s", app_name);
                printf("[EXE] Running application: %s\n", app_name);
                // Would run app here
            } else if (strncmp(line, "beep", 4) == 0) {
                printf("[EXE] Beep!\n");
                sound_beep(800, 500); // 800 Hz for 500 ms
            } else if (strncmp(line, "clear", 5) == 0) {
                printf("\033[2J\033[H"); // ANSI clear screen
            } else if (strncmp(line, "ls", 2) == 0) {
                printf("[EXE] Files:\n");
                printf("  myfile.txt\n");
                printf("  script.ua\n");
                printf("  data.bin\n");
            }
            // Find next line
            while (*line && *line != '\n') line++;
            if (*line == '\n') line++;
        }

        printf("[EXE] Script execution completed\n");
        return 0;
    } else {
        printf("[EXE] Unsupported file format\n");
        printf("[EXE] UA-Horizon supports scripts with #!/ua-horizon header\n");
        return -2;
    }
}

int exe_create_demo(const char *filename) {
    const char *script =
        "#!/ua-horizon\n"
        "print UA-Horizon Demo Script\n"
        "print ====================\n"
        "print This is a demo executable script!\n"
        "delay 100\n"
        "print Script completed successfully!\n";

    fs_create_file(filename, strlen(script));
    fs_write_file(filename, script, strlen(script));

    printf("[EXE] Created demo script: %s\n", filename);
    return 0;
}