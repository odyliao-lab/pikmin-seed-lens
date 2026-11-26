"""Compare reviewed native profile records against actual ELF and full-signature dump.
No device operations, no automatic source changes, no compatibility certification.
"""
import argparse
import copy
import json
import re
import struct
from pathlib import Path
import profile_evidence as evidence

REVIEWED_CODES={'156.0':1792142832,'156.1':1793076318,'157.0':1793360009,'158.0':1794902053}
DISPLAY_HOOKS={
    'honey': ('Niantic.Ichigo.Game.Garden.Extracts.Scroll','HoneyBallEntry',
              'public void UpdateData(HoneyBallData data) { }'),
    'extract': ('Niantic.Ichigo.Game.Garden.Extracts','ExtractBallEntry',
                'private Task UpdateDataAsync(HoneyBallInventoryItem item) { }'),
    'adult': ('Niantic.Ichigo.Game.PikminManagementV2.Entries','PikminListEntry',
              'private void SetupInfo(PikminInventoryItem pikmin) { }'),
    'planted_detail': ('Niantic.Ichigo.Game.PikminOrSeedDetailsView','PikminOrSeedDetailsViewTitle',
                       'protected override void Refresh(PikminOrSeed init) { }'),
    'login': ('Niantic.Ichigo.Rpc','RpcManager',
              'public void OnLoggedIn(string serverUrl, string playerId, ByteString {token}) {{ }}'),
    'logout': ('Niantic.Ichigo.Rpc','RpcManager','public void OnLoggedOut() { }'),
}
DISPLAY_LOGIN_PARAMETER={'157.0':'backgroundApiToken','158.0':'token'}
INVENTORY_MANAGER=('Niantic.Ichigo.Inventory','InventoryManager')
INVENTORY_FIELDS=(
    'private long latestServerSideTimestamp',
    'private readonly Dictionary<string, Predicted<InventoryItem>> itemsById',
)
GAME158_METADATA_SHA='2de78f3584018ab341dd6b85281453d5b765a66558448ef43f4e0494afd00949'
HOOK_ROW=r'\{0x[0-9a-f]+,\{(?:0x[0-9a-f]+,?)+\}\},?\s*'

def parse_hooks(hooks_text, keys):
    hooks=re.findall(r'\{(0x[0-9a-f]+),\{([^}]+)\}\}',hooks_text)
    if len(hooks)!=len(keys) or any(len(words.split(','))!=6 for _,words in hooks):
        raise ValueError('incomplete hook prefix')
    return {key:(int(rva,16),[int(x,16) for x in words.split(',')]) for key,(rva,words) in zip(keys,hooks)}

def native_profiles(text):
    array=re.search(r'inline constexpr GameProfile game_profiles\[\]\s*\{(.*?)\n\};',text,re.S)
    if not array:raise ValueError('reviewed profile array missing')
    body=array[1]
    pattern=rf'\{{"(\d+\.\d+)",(\d+),(\d+),"([a-f0-9]{{64}})",(\d+),\s*((?:{HOOK_ROW}){{5}})\}}'
    records={};at=0
    for match in re.finditer(pattern,body):
        if body[at:match.start()].strip()!=(',' if records else ''):
            raise ValueError('unparsed profile declaration')
        at=match.end()
        version,code,size,sha,layout,hooks_text=match.groups()
        if version in records:raise ValueError('duplicate profile')
        records[version]={'version_code':int(code),'size':int(size),'sha':sha,'layout':int(layout),
                          'hooks':parse_hooks(hooks_text,evidence.HOOKS)}
    if body[at:].strip() not in ('',','):raise ValueError('unparsed profile declaration')
    if set(records)!=set(REVIEWED_CODES):raise ValueError('incomplete/unreviewed profile set')
    if any(records[version]['version_code']!=code for version,code in REVIEWED_CODES.items()):
        raise ValueError('reviewed version/code mismatch')
    return records

def display_profiles(text):
    array=re.search(r'inline constexpr DisplayGameProfile display_profiles\[\]\s*\{(.*?)\n\};',text,re.S)
    if not array:raise ValueError('reviewed display profile array missing')
    body=array[1]
    pattern=rf'\{{"(\d+\.\d+)",(0x[0-9a-f]+),(0x[0-9a-f]+),\s*((?:{HOOK_ROW}){{6}})\}}'
    records={};at=0
    for match in re.finditer(pattern,body):
        if body[at:match.start()].strip()!=(',' if records else ''):
            raise ValueError('unparsed display profile declaration')
        at=match.end()
        version,revision,items,hooks_text=match.groups()
        if version in records:raise ValueError('duplicate display profile')
        records[version]={'inventory_fields':(int(revision,16),int(items,16)),
                          'hooks':parse_hooks(hooks_text,DISPLAY_HOOKS)}
    if body[at:].strip() not in ('',',') or set(records)!=set(DISPLAY_LOGIN_PARAMETER):
        raise ValueError('incomplete/unreviewed display profile set')
    return records

def verify(profile,result,elf):
    if profile['layout']!=1 or profile['size']!=result['inputs']['elf']['size'] or profile['sha']!=result['inputs']['elf']['sha256']:
        raise ValueError('binary identity mismatch')
    verify_hooks(profile['hooks'],result,elf)

def verify_hooks(hooks,result,elf):
    for key,(rva,words) in hooks.items():
        expected=result['hooks'][key]['rva']
        if key=='text':
            _,branch=evidence.prefix(elf,expected,4)
            if branch!=struct.pack('<I',0x14000001):raise ValueError('text veneer changed')
            expected+=4
        if rva!=expected:raise ValueError('wrong full-signature entry: '+key)
        _,actual=evidence.prefix(elf,rva)
        if list(struct.unpack('<6I',actual))!=words:raise ValueError('prologue mismatch: '+key)

def verify_display_fields(profile,result):
    fields=result['layouts']['.'.join(INVENTORY_MANAGER)]['fields']
    for name,offset in zip(INVENTORY_FIELDS,profile['inventory_fields']):
        if [field['offset'] for field in fields if field['declaration']==name]!=[offset]:
            raise ValueError('inventory field layout mismatch: '+name)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--game1560',type=Path,required=True);p.add_argument('--game1561',type=Path,required=True)
    p.add_argument('--game1570',type=Path,required=True);p.add_argument('--game1580',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    root=Path(__file__).resolve().parents[1]
    header=root/'native/renderer/game_profiles.h';profiles=native_profiles(header.read_text())
    display_header=root/'native/renderer/extended_display_profile.h';displays=display_profiles(display_header.read_text())
    results={};rejected=0
    for version,folder in [('156.0',a.game1560),('156.1',a.game1561),('157.0',a.game1570),('158.0',a.game1580)]:
        signatures=dict(evidence.HOOKS)
        classes=set(evidence.CLASSES)
        if version in displays:
            signatures.update(DISPLAY_HOOKS)
            ns,cls,signature=signatures['login']
            signatures['login']=(ns,cls,signature.format(token=DISPLAY_LOGIN_PARAMETER[version]))
            classes.update((ns,cls) for ns,cls,_ in signatures.values())
            classes.add(INVENTORY_MANAGER)
        profile=profiles[version];elf=folder/'libil2cpp.so'
        result=evidence.evidence(folder/'dump/dump.cs',elf,folder/'global-metadata.dat',signatures,classes)
        if version=='158.0' and result['inputs']['metadata']['sha256']!=GAME158_METADATA_SHA:
            raise ValueError('158.0 metadata identity mismatch')
        if version in displays:
            display=displays[version]
            profile={**profile,'hooks':{**profile['hooks'],**display['hooks']}}
            verify_display_fields(display,result)
            for index in range(len(INVENTORY_FIELDS)):
                fields=list(display['inventory_fields']);fields[index]+=8
                try:verify_display_fields({'inventory_fields':fields},result)
                except ValueError:rejected+=1
                else:raise AssertionError('missed inventory field mutation')
        verify(profile,result,elf);results[version]=result
        mutations=[]
        for key in profile['hooks']:
            bad=copy.deepcopy(profile);rva,words=bad['hooks'][key];bad['hooks'][key]=(rva+4,words);mutations.append(bad)
            bad=copy.deepcopy(profile);bad['hooks'][key][1][0]^=1;mutations.append(bad)
        for key,value in [('size',profile['size']+1),('sha','0'*64),('layout',2)]:
            bad=copy.deepcopy(profile);bad[key]=value;mutations.append(bad)
        for bad in mutations:
            try:verify(bad,result,elf)
            except ValueError:rejected+=1
            else:raise AssertionError('missed source mutation')
    fields=lambda result:{k:result['layouts'][k]['fields'] for k in map('.'.join,evidence.CLASSES)}
    if any(fields(result)!=fields(results['156.0']) for result in results.values()):
        raise ValueError('shared-layout assumption failed')
    report={'status':'OFFLINE_SOURCE_MATCH_NOT_DEVICE_ACCEPTANCE','header_sha256':evidence.sha(header),
            'display_header_sha256':evidence.sha(display_header),
            'shared_layout_classes':len(results['156.0']['layouts']),'rejected_mutations':rejected,'games':results}
    with a.output.open('x',encoding='utf-8') as f:json.dump(report,f,indent=2)
    count=len(results)
    hook_count=sum(len(result['hooks']) for result in results.values())
    print(f'PASS: {count} exact ELF identities, {hook_count} full-signature hook bodies/prologues, {count} text veneers, {len(evidence.CLASSES)} shared class layouts, {len(displays)} extended inventory layouts, {rejected} rejected mutations; NOT device acceptance')

if __name__=='__main__':main()
