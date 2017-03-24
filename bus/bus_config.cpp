// Generated by tool, DO NOT EDIT!!!

#include "bus_config.h"
#include "pugixml.hpp"
#include "utility/types.h"
#include "utility/assert_helper.h"
#include "utility/string_helper.h"

static int load_from_xml_node(size_t& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(bus_config& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(std::string& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(std::vector<std::string>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name);
static int load_from_xml_node(s32& value, const pugi::xml_node& node, const char *node_name);


static int load_from_xml_node(size_t& value, const pugi::xml_node& node, const char *node_name) {
    value = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    int ret = sk::string_traits<u64>::from_string(node.text().get(), value);
    if (ret != 0) {
        fprintf(stderr, "cannot convert node %s to integer.\n", node_name);
        return ret;
    }

    return 0;
}

static int load_from_xml_node(bus_config& value, const pugi::xml_node& node, const char *node_name) {
    int ret = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    // read fields

    ret = load_from_xml_node(value.bus_shm_key, node.child("bus_shm_key"), "bus_shm_key");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.msg_per_run, node.child("msg_per_run"), "msg_per_run");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.listen_port, node.child("listen_port"), "listen_port");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.shm_size, node.child("shm_size"), "shm_size");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.bus_shm_size, node.child("bus_shm_size"), "bus_shm_size");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.report_interval, node.child("report_interval"), "report_interval");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.consul_addr_list, node.children("consul_addr_list"), "consul_addr_list");
    if (ret != 0)
        return ret;

    return 0;
}

static int load_from_xml_node(std::string& value, const pugi::xml_node& node, const char *node_name) {
    value.clear();
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }
    value = node.text().get();

    return 0;
}

static int load_from_xml_node(std::vector<std::string>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name) {
    value.clear();
    for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
        const pugi::xml_node n = *it;
        std::string obj;
        int ret = load_from_xml_node(obj, n, node_name);
        if (ret != 0)
            return ret;

        value.push_back(obj);
    }

    /*
     * do NOT do this check, some selective configurations might
     * be totally optional, so it's possible nothing is provided
     */
    // if (value.size() <= 0) {
    //     fprintf(stderr, "node %s not found.\n", node_name);
    //     return -EINVAL;
    // }

    return 0;
}

static int load_from_xml_node(s32& value, const pugi::xml_node& node, const char *node_name) {
    value = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    int ret = sk::string_traits<s32>::from_string(node.text().get(), value);
    if (ret != 0) {
        fprintf(stderr, "cannot convert node %s to integer.\n", node_name);
        return ret;
    }

    return 0;
}

int bus_config::load_from_xml_file(const char *filename) {
    assert_retval(filename, -1);

    pugi::xml_document doc;
    auto ok = doc.load_file(filename);
    if (!ok) {
        fprintf(stderr, "load file %s error: %s.\n", filename, ok.description());
        return -EINVAL;
    }

    pugi::xml_node root = doc.child("bus_config");
    if (!root) {
        fprintf(stderr, "root node <bus_config> not found.\n");
        return -EINVAL;
    }

    return load_from_xml_node(*this, root, "bus_config");
}