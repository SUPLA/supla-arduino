set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")


# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
set(COMPONENT_EMBED_TXTFILES supla_org_cert.pem)

register_component()
