include $(TOPDIR)/rules.mk
PKG_NAME:=UDP_1907
PKG_VERSION:=1.0.6
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/UDP_1907
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=UDP_1907 ---------- HC_RD_vuhongtu
	DEPENDS:= +libjson-c +libstdcpp
endef

define Package/UDP_1907/description
 If you can't figure out what this program does,
 you're probably brain-dead and need immediate
 medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/UDP_1907/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/UDP_1907 $(1)/bin/
endef

$(eval $(call BuildPackage,UDP_1907))
