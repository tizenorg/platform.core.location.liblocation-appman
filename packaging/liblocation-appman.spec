%define _optdir /opt
Name:       liblocation-appman
Summary:    Location DB controler
Version:    0.1.6
Release:    0
Group:      System/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: liblocation-appman.manifest

Requires(post): sqlite
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  libtzplatform-config-devel
BuildRequires:  libgcrypt-devel

%description
Description: Location DB controller library


%package devel
Summary:    Location DB controller (Development files)
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Description : Location DB controller Development Package


%prep
%setup -q -n %{name}-%{version}
cp %{SOURCE1001} .


%build
%cmake . -DTZ_SYS_DB=%{TZ_SYS_DB}

# Call make instruction with smp support
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

chown root:%{TZ_SYS_USER_GROUP} %{TZ_SYS_DB}/.location-appman.db*
chmod 664 %{TZ_SYS_DB}/.location-appman.db*
%postun -p /sbin/ldconfig

%install
rm -rf %{buildroot}
%make_install


%clean
rm -rf %{buildroot}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/liblocation-appman.so*
%{TZ_SYS_DB}/.location-appman.db*

%files devel
%manifest %{name}.manifest
%{_includedir}/location-appman/*.h
%{_libdir}/pkgconfig/*.pc
